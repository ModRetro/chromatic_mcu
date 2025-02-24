#include "serial_num.h"

#include "osd_shared.h"
#include "lvgl.h"
#include "esp_log.h"

#if defined(ESP_PLATFORM)
#include "esp_console.h"
#include "esp_efuse.h"
#endif

#include <stdbool.h>
#include <string.h>

enum {
    kSNBufferLen = 11, // Accounts for NUL terminator
    kNumSNChars = kSNBufferLen - 1,

    kTextX_px = 80,
    kTextY_px = 45,
};

static const lv_point_t SNOrigin = {.x = kTextX_px, .y = kTextY_px };

typedef struct SerialNumber {
    lv_obj_t* pSerialNumObj;
    char SerialNumStr[kSNBufferLen];
} SerialNumber_t;

static SerialNumber_t _Ctx;
static const char* TAG = "fw";
static const char SNNotAvailable[kSNBufferLen] = "0000000000";
static const char SNNotWritten[kSNBufferLen] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#if defined(ESP_PLATFORM)
static int serial_num_command(int argc, char **argv);
static OSD_Result_t register_serial_num_dump(void);
#endif

OSD_Result_t SerialNum_Draw(void* arg)
{
    if (arg == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    lv_obj_t *const pScreen = (lv_obj_t *const) arg;


    if (_Ctx.pSerialNumObj == NULL)
    {
        _Ctx.pSerialNumObj = lv_label_create(pScreen);

        // Print the firmware version
        lv_obj_add_style(_Ctx.pSerialNumObj, OSD_GetStyleTextWhite(), 0);
        lv_obj_align(_Ctx.pSerialNumObj, LV_ALIGN_TOP_LEFT, SNOrigin.x, SNOrigin.y);
        lv_label_set_text(_Ctx.pSerialNumObj, (const char*)_Ctx.SerialNumStr);
    }
    else
    {
        lv_obj_move_foreground(_Ctx.pSerialNumObj);
    }

    return kOSD_Result_Ok;
}

OSD_Result_t SerialNum_OnTransition(void* arg)
{
    (void)(arg);

    const lv_obj_t** ToDelete[] = {
        (const lv_obj_t**)&_Ctx.pSerialNumObj,
    };

    for (size_t i = 0; i < ARRAY_SIZE(ToDelete); i++)
    {
        if (*ToDelete[i] != NULL)
        {
            lv_obj_del((lv_obj_t*)*ToDelete[i]);
            *(ToDelete[i]) = NULL;
        }
    }

    return kOSD_Result_Ok;
}

OSD_Result_t SerialNum_Initialize(void)
{
    OSD_Result_t eResult = kOSD_Result_Ok;

#if defined(ESP_PLATFORM)
    const esp_efuse_desc_t SerialNumDesc[] = {
        {EFUSE_BLK3, 3, 80},
    };
    const esp_efuse_desc_t *pEFuses[] = { &SerialNumDesc[0], NULL };
    const esp_err_t err = esp_efuse_read_field_blob(pEFuses, _Ctx.SerialNumStr, kNumSNChars * 8);

    // The block read failed.
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read serial number: %s", esp_err_to_name(err));
        eResult = kOSD_Result_Err_FailedToReadSN;

        strncpy(_Ctx.SerialNumStr, SNNotAvailable, kSNBufferLen);
    }
    else
    {
        // The read was successful but the serial number was not written
        if (memcmp(_Ctx.SerialNumStr, SNNotWritten, kSNBufferLen) == 0 )
        {
            strncpy(_Ctx.SerialNumStr, SNNotAvailable, kSNBufferLen);
        }

        eResult = register_serial_num_dump();
    }

    _Ctx.SerialNumStr[kNumSNChars] = '\0';
#else
    lv_snprintf(_Ctx.SerialNumStr, kSNBufferLen, "SIMULATION");
#endif

    return eResult;
}

bool SerialNum_IsPresent(void)
{
    return (strncmp(_Ctx.SerialNumStr, SNNotAvailable, kSNBufferLen) != 0);
}

#if defined(ESP_PLATFORM)
static int serial_num_command(int argc, char **argv)
{
    printf("{ \"serial_num\": \"%s\" }\r\n", _Ctx.SerialNumStr);

    return 0;
}

static OSD_Result_t register_serial_num_dump(void)
{
    esp_console_cmd_t command = {
        .command = "serialnum",
        .help = "Print the device serial number",
        .func = &serial_num_command,
        .argtable = NULL,
    };

    esp_err_t err;
    if ((err = esp_console_cmd_register(&command)) != ESP_OK)
    {
        ESP_LOGE(TAG, "Registering '%s' command failed: %s", command.command, esp_err_to_name(err));
        return kOSD_Result_Err_CmdRegFailed;
    }

    return kOSD_Result_Ok;
}
#endif
