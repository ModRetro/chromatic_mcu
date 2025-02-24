#pragma once

#include "osd_shared.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum BatteryKind {
    kBatteryKind_AA,
    kBatteryKind_LiPo,
    kNumBatteryKind,
} BatteryKind_t;

typedef enum LiPoChargeStatus {
    kLipoChargeStatus_Disabled,
    kLipoChargeStatus_Enabled,
} LipoChargeStatus_t;

OSD_Result_t Battery_Init(OSD_Widget_t *const pWidget);
void Battery_UpdateVoltage(const float batt_V, const BatteryKind_t eKind);
void Battery_SetChargingStatus(const bool IsCharging);
