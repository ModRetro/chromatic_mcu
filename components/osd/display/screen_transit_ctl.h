#pragma once

#include "osd_shared.h"
#include "settings.h"

typedef enum ScreenTransitCtlState
{
    kScreenTransitCtlState_Off,
    kScreenTransitCtlState_On,
    kNumScreenTransitCtlState,
} ScreenTransitCtlState_t;

OSD_Result_t ScreenTransitCtl_Draw(void* arg);
void ScreenTransitCtl_Update(const ScreenTransitCtlState_t eNewState);
OSD_Result_t ScreenTransitCtl_OnButton(const Button_t Button, const ButtonState_t State, void *arg);
OSD_Result_t ScreenTransitCtl_OnTransition(void* arg);
ScreenTransitCtlState_t ScreenTransitCtl_GetState(void);
OSD_Result_t ScreenTransitCtl_ApplySetting(SettingValue_t const *const pValue);
void ScreenTransitCtl_RegisterOnUpdateCb(fnOnUpdateCb_t fnOnUpdate);
