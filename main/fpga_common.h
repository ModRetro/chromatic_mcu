#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdint.h>

typedef enum {
    kSysMgmtConsts_HeaderV1Marker = 0x8A,
    kSysMgmtConsts_HeaderV2Marker = 0x8F,
    kSysMgmtConsts_MsgProtoV1Len  = 2,
    kSysMgmtConsts_MsgProtoV2Len  = 10,

    kSysMgmtConsts_MsgProtoV2MsgLenOffset = 0x2,

    kSysMgmtConsts_MaxMsgProtoV1Len = 4,
    kSysMgmtConsts_MaxMsgProtoV2Len = 14,
    kSysMgmtConsts_RxBufferSize     = 2 * 10 * kSysMgmtConsts_MaxMsgProtoV2Len + 1,
} SysMgmtConsts_t;

TaskHandle_t* FPGA_GetTxTaskHandle(void);
TaskHandle_t* FPGA_GetRxTaskHandle(void);
bool FPGA_IsProtoV1(void);
void FPGA_SetProtoV1(const bool V1);
