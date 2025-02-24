#pragma once

#include "osd_shared.h"
#include "settings.h"

typedef enum SilentModeState
{
    kSilentModeState_Off,
    kSilentModeState_On,
    kNumSilentModeStates,
} SilentModeState_t;

OSD_Result_t SilentMode_Draw(void* arg);
void SilentMode_Update(const SilentModeState_t eNewState);
OSD_Result_t SilentMode_OnButton(const Button_t Button, const ButtonState_t State, void *arg);
OSD_Result_t SilentMode_OnTransition(void* arg);
SilentModeState_t SilentMode_GetState(void);
OSD_Result_t SilentMode_ApplySetting(SettingValue_t const *const pValue);
void SilentMode_RegisterOnUpdateCb(fnOnUpdateCb_t fnOnUpdate);
