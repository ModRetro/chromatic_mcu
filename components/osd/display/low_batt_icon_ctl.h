#pragma once

#include "osd_shared.h"
#include "settings.h"

typedef enum LowBattIconCtlState
{
    kLowBattIconCtlState_AlwaysShow,
    kLowBattIconCtlState_Flash,
    kLowBattIconCtlState_Hide,
    kNumLowBattIconCtlState,
} LowBattIconCtlState_t;

OSD_Result_t LowBattIconCtl_Draw(void* arg);
void LowBattIconCtl_Update(const LowBattIconCtlState_t eNewState);
OSD_Result_t LowBattIconCtl_OnButton(const Button_t Button, const ButtonState_t State, void *arg);
OSD_Result_t LowBattIconCtl_OnTransition(void* arg);
LowBattIconCtlState_t LowBattIconCtl_GetState(void);
OSD_Result_t LowBattIconCtl_ApplySetting(SettingValue_t const *const pValue);
void LowBattIconCtl_RegisterOnUpdateCb(fnOnUpdateCb_t fnOnUpdate);
