#pragma once

#include "osd_shared.h"
#include "settings.h"

typedef enum ColorCorrectUSBState
{
    kColorCorrectUSBState_Off,
    kColorCorrectUSBState_On,
    kNumColorCorrectUSBState,
} ColorCorrectUSBState_t;

OSD_Result_t ColorCorrectUSB_Draw(void* arg);
void ColorCorrectUSB_Update(const ColorCorrectUSBState_t eNewState);
OSD_Result_t ColorCorrectUSB_OnButton(const Button_t Button, const ButtonState_t State, void *arg);
OSD_Result_t ColorCorrectUSB_OnTransition(void* arg);
ColorCorrectUSBState_t ColorCorrectUSB_GetState(void);
OSD_Result_t ColorCorrectUSB_ApplySetting(SettingValue_t const *const pValue);
void ColorCorrectUSB_RegisterOnUpdateCb(fnOnUpdateCb_t fnOnUpdate);
