#pragma once

#include "osd_shared.h"

typedef enum SettingKey {
    kSettingKey_FrameBlend,
    kSettingKey_ColorCorrectLCD,
    kSettingKey_ColorCorrectUSB,
    kSettingKey_PlayerNum,
    kSettingKey_SilentMode,
    kSettingKey_Brightness,
    kSettingKey_ScreenTransitCtl,
    kSettingKey_DPadCtl,
    kSettingKey_LowBattIconCtl,
    kNumSettingKeys,

    kSettingKey_FirstKey = kSettingKey_FrameBlend,
} SettingKey_t;

typedef enum SettingDataType {
    kSettingDataType_U8,
    kSettingDataType_I8,
    kSettingDataType_U16,
    kSettingDataType_I16,
    kSettingDataType_U32,
    kSettingDataType_I32,
} SettingDataType_t;

typedef struct SettingValue {
    SettingDataType_t eType;
    union {
        uint8_t U8;
        int8_t I8;
        uint16_t U16;
        int16_t I16;
        uint32_t U32;
        int32_t I32;
    };
} SettingValue_t;

typedef OSD_Result_t (*fnSettingApply_t)(SettingValue_t const *const pValue);

OSD_Result_t Settings_Initialize(void);
OSD_Result_t Settings_Update(const SettingKey_t eKey, const uint32_t Value);
OSD_Result_t Settings_Retrieve(const SettingKey_t eKey, uint32_t *const pValue);
const char* Settings_GetNVSNamespace(void);
const char* Settings_KeyToName(const SettingKey_t eKey);
OSD_Result_t Settings_RetrieveDefault(const SettingKey_t eKey, SettingValue_t *const pValue);
