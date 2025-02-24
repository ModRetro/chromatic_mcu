#pragma once

#include "osd_shared.h"
#include "settings.h"

typedef enum ColorCorrectLCDState
{
    kColorCorrectLCDState_Off,
    kColorCorrectLCDState_On,
    kNumColorCorrectLCDState,
} ColorCorrectLCDState_t;

OSD_Result_t ColorCorrectLCD_Draw(void* arg);
void ColorCorrectLCD_Update(ColorCorrectLCDState_t eNewState);
OSD_Result_t ColorCorrectLCD_OnButton(const Button_t Button, const ButtonState_t State, void *arg);
OSD_Result_t ColorCorrectLCD_OnTransition(void* arg);
ColorCorrectLCDState_t ColorCorrectLCD_GetState(void);
OSD_Result_t ColorCorrectLCD_ApplySetting(SettingValue_t const *const pValue);
void ColorCorrectLCD_RegisterOnUpdateCb(fnOnUpdateCb_t fnOnUpdate);
