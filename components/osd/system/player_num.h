#pragma once

#include "osd_shared.h"
#include "settings.h"

OSD_Result_t PlayerNum_Draw(void* arg);
void PlayerNum_Update(const uint8_t NewNum);
OSD_Result_t PlayerNum_OnButton(const Button_t Button, const ButtonState_t State, void *arg);
OSD_Result_t PlayerNum_OnTransition(void* arg);
uint8_t PlayerNum_GetNum(void);
OSD_Result_t PlayerNum_ApplySetting(SettingValue_t const *const pValue);
void PlayerNum_RegisterOnUpdateCb(fnOnUpdateCb_t fnOnUpdate);
