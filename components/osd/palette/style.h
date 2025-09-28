#pragma once

#include "osd_shared.h"
#include "settings.h"

typedef enum StyleID{
    kPalette_Default,
    kPalette_Brown,
    kPalette_Blue,
    kPalette_Pastel,
    kPalette_Green,
    kPalette_Red,
    kPalette_DarkBlue,
    kPalette_Orange,
    kPalette_DarkGreen,
    kPalette_DarkBrown,
    kPalette_Grayscale,
    kPalette_Yellow,
    kPalette_Negative,
    kPalette_DMG1,
    kPalette_DMG2,

    kNumPalettes,
    kCustomPaletteEn = 63  // Custom palette bit; when set the chosen palette is enabled
} StyleID_t;

OSD_Result_t Style_Draw(void* arg);
void Style_Update(const StyleID_t NewID);
OSD_Result_t Style_OnButton(const Button_t Button, const ButtonState_t State, void *arg);
OSD_Result_t Style_OnTransition(void* arg);
uint64_t Style_GetPaletteBG(const StyleID_t ID);
StyleID_t Style_GetCurrID(void);
OSD_Result_t Style_ApplySetting(const SettingValue_t* pValue);
void Style_RegisterOnUpdateCb(fnOnUpdateCb_t fnOnUpdate);
void Style_SetGBCMode(const bool GBCMode);
void Style_SetHKPaletteBG(const uint64_t paletteBG);
bool Style_IsInitialized(void);
void Style_Initialize(void);

