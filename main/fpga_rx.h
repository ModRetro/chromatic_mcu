#pragma once

#include "fpga_common.h"

typedef enum {
    kRxCmd_VoltageAA       = 0x0,
    kRxCmd_VoltageLiPo     = 0x1,
    kRxCmd_Buttons         = 0x2,
    kRxCmd_AudioBrightness = 0x3,
    kRxCmd_SysCtl          = 0x4,
    kRxCmd_PMIC            = 0x5,
    kRxCmd_FWVer           = 0x6,
    kRxCmd_Reserved        = 0x7,
    kRxCmd_StatusExtended  = 0x8,

    kNumRxCmds,
} RxIDs_t;

typedef enum {
    kFPGA_RxConsts_BufferSize = 1024,    // [bytes]
} FPGA_RxConsts_t;

void FPGA_RxTask(void *arg);
void FPGA_Rx_Resume(void);
void FPGA_Rx_Pause(void);
void FPGA_Rx_UseBrightnessReadback(void);
