#include "fpga_common.h"
#include "freertos/task.h"

#include <stddef.h>

static TaskHandle_t TxTaskHandle = NULL;
static TaskHandle_t RxTaskHandle = NULL;
static bool IsV1 = false; // Default to V2

TaskHandle_t* FPGA_GetTxTaskHandle(void)
{
    return &TxTaskHandle;
}

TaskHandle_t* FPGA_GetRxTaskHandle(void)
{
    return &RxTaskHandle;
}

bool FPGA_IsProtoV1(void)
{
    return IsV1;
}

void FPGA_SetProtoV1(const bool V1)
{
    IsV1 = V1;
}
