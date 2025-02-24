#pragma once

#include "osd_shared.h"
#include "settings.h"

OSD_Result_t Brightness_Draw(void* arg);
void Brightness_Update(const uint8_t Brightness);
OSD_Result_t Brightness_OnTransition(void* arg);
OSD_Result_t Brightness_OnButton(const Button_t Button, const ButtonState_t State, void *arg);
uint8_t Brightness_GetLevel(void);
OSD_Result_t Brightness_ApplySetting(SettingValue_t const *const pValue);
void Brightness_RegisterOnUpdateCb(fnOnUpdateCb_t fnOnUpdate);
void Brightness_SetLowPowerOverride(const bool Enable);
