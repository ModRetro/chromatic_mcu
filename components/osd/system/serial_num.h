#pragma once

#include "osd_shared.h"

#include <stdbool.h>

OSD_Result_t SerialNum_Draw(void* arg);
OSD_Result_t SerialNum_OnTransition(void* arg);
OSD_Result_t SerialNum_Initialize(void);
bool SerialNum_IsPresent(void);
