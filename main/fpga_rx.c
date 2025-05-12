#include "fpga_rx.h"

#include "battery.h"
#include "brightness.h"
#include "color_correct_lcd.h"
#include "color_correct_usb.h"
#include "crc8_sae_j1850.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"
#include "fpga_common.h"
#include "fpga_tx.h"
#include "frameblend.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fw.h"
#include "osd.h"
#include "pwrmgr.h"
#include "player_num.h"
#include "silent.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef enum {
    kRxFlag_Resume = (1 << 0),
    kRxFlag_UseBrightness = (1 << 1),
} RxFlags_t;

typedef enum {
    kScanForHeaderMarker,
    kWaitForFullV1Msg,

    kWaitForV2Addr,
    kWaitForV2Len,
    kWaitForV2Payload,
    kWaitForV2CRC,
} RxState_t;

typedef struct __attribute__((packed)) RxMsg {
    uint8_t Header;
    uint8_t Addr;
    uint8_t Len;
    union {
        uint8_t Payload[kSysMgmtConsts_MsgProtoV2Len];
        uint32_t PayloadU32;
    };
    uint8_t CRC;
} RxMsg_t;

static const char* TAG = "FpgaRx";
static RxState_t _eState = kScanForHeaderMarker;
static uint8_t DecodeBuffer[kSysMgmtConsts_MaxMsgProtoV2Len];
static uint32_t BufferIndex = 0;
static uint8_t RxBuffer[kSysMgmtConsts_RxBufferSize]; // Allocate enough room to store several messages

// Declare a variable to hold the handle of the created event group.
static EventGroupHandle_t xEventGroupHandle = NULL;

// Declare a variable to hold the data associated with the created event group.
static StaticEventGroup_t xCreatedEventGroup;

static inline bool IsHeaderMarker(const uint8_t NextByte);
static bool ReconstructMsg(uint8_t *pBuffer, RxMsg_t *pMsg);
static bool ProcessByte(uint8_t NextByte, RxMsg_t *pMsg);
static void ProcessMessage(const RxMsg_t *const pMsg);

void FPGA_RxTask(void *arg)
{
    (void)arg;

    xEventGroupHandle = xEventGroupCreateStatic( &xCreatedEventGroup );

    uint8_t *const pRxBuffer = RxBuffer;
    if (pRxBuffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate read buffer. Aborting!");
        return;
    }

    FPGA_Rx_Resume();

    RxMsg_t Msg = {0};
    while (1)
    {
        //Only run when we're given permission to do so.
        (void) xEventGroupWaitBits(
            xEventGroupHandle,
            kRxFlag_Resume,
            pdFALSE,        // Do not clear the Resume flag
            pdTRUE,         // Wait for this bit
            portMAX_DELAY
        );

        const int32_t ByteCount = uart_read_bytes(UART_NUM_1, pRxBuffer, sizeof(RxBuffer), pdMS_TO_TICKS(10));

        if (ByteCount > 0)
        {
            // The system will enter a low power mode if enough time passes without any received serial data to save power.
            PwrMgr_IdleTimerPet();
        }

        for (size_t i = 0; i < ByteCount; i++)
        {
            if (ProcessByte(pRxBuffer[i], &Msg))
            {
                ProcessMessage(&Msg);
                memset(&Msg, 0x0, sizeof(Msg));
            }
        }
    }

    ESP_LOGE(TAG, "RxTask loop exited");

    free(pRxBuffer);
}

void FPGA_Rx_Resume(void)
{
   (void) xEventGroupSetBits(xEventGroupHandle, kRxFlag_Resume);
}

void FPGA_Rx_Pause(void)
{
   (void) xEventGroupClearBits(xEventGroupHandle, kRxFlag_Resume);
}

void FPGA_Rx_UseBrightnessReadback(void)
{
   (void) xEventGroupSetBits(xEventGroupHandle, kRxFlag_UseBrightness);
}

static bool ReconstructMsg(uint8_t *pBuffer, RxMsg_t *pMsg)
{
    if ((pMsg == NULL) || (pBuffer == NULL))
    {
        return false;
    }

    pMsg->Header  = pBuffer[0];
    pMsg->Addr    = pBuffer[1];

    if (pMsg->Header == kSysMgmtConsts_HeaderV1Marker)
    {
        pMsg->Len = kSysMgmtConsts_MsgProtoV1Len;
        pMsg->Payload[0] = pBuffer[3];
        pMsg->Payload[1] = pBuffer[2];
    }
    else if (pMsg->Header == kSysMgmtConsts_HeaderV2Marker)
    {
        const uint8_t LenOffset = 2;
        const uint8_t Len = pBuffer[LenOffset];

        pMsg->Len = Len;

        memcpy(pMsg->Payload, &pBuffer[LenOffset + 1], Len);
        pMsg->CRC = pBuffer[LenOffset + 1 + Len];
    }

    return true;
}

static bool ProcessByte(uint8_t NextByte, RxMsg_t *pMsg)
{
    static uint8_t MsgLen = 0;

    if (pMsg == NULL)
    {
        return false;
    }

    // printf(" %X\r\n", NextByte);

    switch (_eState)
    {
        case kScanForHeaderMarker:
            if (IsHeaderMarker(NextByte))
            {
                DecodeBuffer[0] = NextByte;
                BufferIndex = 1;
                MsgLen = 0;

                if (NextByte == kSysMgmtConsts_HeaderV2Marker)
                {
                    _eState = kWaitForV2Addr;
                    FPGA_SetProtoV1(false);
                }
                else
                {
                    _eState = kWaitForFullV1Msg;
                    FPGA_SetProtoV1(true);
                }
            }
            break;

        case kWaitForFullV1Msg:
            if (NextByte == kSysMgmtConsts_HeaderV1Marker)
            {
                // We did not receive a complete message. Start over.
                _eState = kScanForHeaderMarker;
                BufferIndex = 0;
                break;
            }

            DecodeBuffer[BufferIndex++] = NextByte;
            if (BufferIndex == kSysMgmtConsts_MaxMsgProtoV1Len)
            {
                _eState = kScanForHeaderMarker;
                BufferIndex = 0;

                return ReconstructMsg(DecodeBuffer, pMsg);
            }
            break;
        case kWaitForV2Addr:
            DecodeBuffer[BufferIndex++] = NextByte;
            _eState = kWaitForV2Len;
            break;
        case kWaitForV2Len:
            if (NextByte > kSysMgmtConsts_MsgProtoV2Len)
            {
                ESP_LOGE(TAG, "Msg for addr %d with len %d exceeds limit %d", DecodeBuffer[BufferIndex - 1], NextByte, kSysMgmtConsts_MsgProtoV2Len);
                _eState = kScanForHeaderMarker;
                BufferIndex = 0;
                break;
            }
            DecodeBuffer[BufferIndex++] = NextByte;
            MsgLen = NextByte;
            _eState = kWaitForV2Payload;
            break;
        case kWaitForV2Payload:
            DecodeBuffer[BufferIndex++] = NextByte;
            if (BufferIndex > (kSysMgmtConsts_MsgProtoV2MsgLenOffset + MsgLen))
            {
                _eState = kWaitForV2CRC;
            }
            break;
        case kWaitForV2CRC:
            DecodeBuffer[BufferIndex++] = NextByte;
            if (crc8_sae_j1850_decode(DecodeBuffer, BufferIndex))
            {
                _eState = kScanForHeaderMarker;
                BufferIndex = 0;
                return ReconstructMsg(DecodeBuffer, pMsg);
            }
            else
            {
                ESP_LOGE(TAG, "CRC check failed for addr=%02x", DecodeBuffer[1]);
            }

            _eState = kScanForHeaderMarker;
            BufferIndex = 0;
            break;
        default:
            break;
    }

    return false;  // No valid message reconstructed yet
}

static void ProcessMessage(const RxMsg_t *const pMsg)
{
    if (pMsg == NULL)
    {
        return;
    }

    // Valid message reconstructed, process the message
    float result;
    uint16_t rxdata;
    if (pMsg->Header == kSysMgmtConsts_HeaderV1Marker)
    {
        // V1 of the protocol has a weird quirk where only the lower 7-bits are used. I suspect it's because it
        // keeps the header marker unique (0x8A) which has the most significant bit set.
        rxdata = ((uint16_t)pMsg->Payload[1] << 7 | pMsg->Payload[0]);
    }
    else
    {
        rxdata = ((uint16_t)pMsg->Payload[0] << 8 | pMsg->Payload[1]);
    }

    switch(pMsg->Addr)
    {
        case kRxCmd_VoltageAA:
        {
            result = (float)rxdata/2048.0f;
            result = (10.0f + 2.2f)*(result/(2.2f));
            float aa_voltage = result - 0.1f;
            Battery_UpdateVoltage(aa_voltage, kBatteryKind_AA);
            break;
        }
        case kRxCmd_VoltageLiPo:
        {
            result = (float)rxdata/2048.0f;
            result = (10.0f + 2.2f)*(result/(2.2f));
            float lipo_voltage = result - 0.1f;
            Battery_UpdateVoltage(lipo_voltage, kBatteryKind_LiPo);
            break;
        }
        case kRxCmd_Buttons:
            Button_Update(rxdata);
            if ((rxdata & kButtonBits_MenuEnAlt) != 0 || (rxdata & kButtonBits_MenuEn) != 0)
            {
                OSD_SetVisiblityState(false);
            }
            else
            {
                // Hack: If we're getting button data, then the OSD is displayed
                OSD_SetVisiblityState(true);
            }
            break;
        case kRxCmd_AudioBrightness:
        {
            uint8_t temp = 127-(rxdata & 0x7F);
            temp = (uint8_t)(((float)temp)*1.08695652173913f);
            const float volume = temp > 100 ? 100 : temp;
            const uint8_t headphone = (rxdata >> 7) & 0x1;
            const uint8_t lcd_brightness = rxdata >> 8;

            (void)volume;
            (void)headphone;

            // Ignore this when the OSD is active since the MCU always has the latest brightness value
            if (OSD_IsVisible() == false)
            {
                if (PwrMgr_IsLPMActive() == false)
                {
                    Brightness_Update(lcd_brightness + 1);  // Brightness module expects 1-indexed data but the display uses zero-indexed
                }
            }

            if ((xEventGroupWaitBits(xEventGroupHandle, kRxFlag_UseBrightness, pdTRUE, pdFALSE, 0) & kRxFlag_UseBrightness) == kRxFlag_UseBrightness)
            {
                FPGA_Tx_SendAll();
                FPGA_Tx_Resume();
            }
            break;
        }
        case kRxCmd_SysCtl:
            (void) rxdata;
            break;
        case kRxCmd_PMIC:
        {
            const uint16_t pmic = rxdata;
            const uint16_t kmChargingStatus = (0x3 << 4);
            const uint16_t kChargingOn = (0x2 << 4);
            const bool IsCharging = ((pmic & kmChargingStatus) == kChargingOn);
            Battery_SetChargingStatus(IsCharging);
            break;
        }
        case kRxCmd_FWVer:
        {
            const uint8_t fpga_debug = ( ((rxdata >> 12) & 1) == 1);
            const uint8_t fpga_version_minor = (uint8_t)((rxdata >> 6) & 0x3F);
            const uint8_t fpga_version_major = (uint8_t)((rxdata >> 0) & 0x3F);
            Firmware_SetFPGAVersion(fpga_version_major, fpga_version_minor, fpga_debug);
            break;
        }
        case kRxCmd_StatusExtended:
        {
            const bool InLPM = (bool)(pMsg->PayloadU32 & 1);

            PwrMgr_SetLPM(InLPM);
            break;
        }
        case kRxCmd_Reserved:
        {
            break;
        }
        default:
            ESP_LOGE(TAG, "Unknown cmd: %d", pMsg->Addr);
            break;
    }

    Brightness_SetLowPowerOverride(PwrMgr_IsLPMActive());
}

static inline bool IsHeaderMarker(const uint8_t NextByte)
{
    return ((NextByte == kSysMgmtConsts_HeaderV1Marker) || (NextByte == kSysMgmtConsts_HeaderV2Marker));
}
