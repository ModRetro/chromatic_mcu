#include "settings.h"

#include "osd_shared.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

enum {
    kNoHandle = 0,
};

// NVS key strings used to RW into the key store. Do not change!
static const char* NVS_KeyStr[kNumSettingKeys] = {
    [kSettingKey_FrameBlend]       = "frame_blending",
    [kSettingKey_ColorCorrectLCD]  = "cc_lcd",
    [kSettingKey_ColorCorrectUSB]  = "cc_usb",
    [kSettingKey_PlayerNum]        = "player_num",
    [kSettingKey_SilentMode]       = "is_muted",
    [kSettingKey_Brightness]       = "backlight",
    [kSettingKey_ScreenTransitCtl] = "screen_transit",
    [kSettingKey_DPadCtl]          = "dpad-diag",
    [kSettingKey_LowBattIconCtl]   = "lbi-ctl",
};

const SettingValue_t DefaultSettings[kNumSettingKeys] = {
    [kSettingKey_FrameBlend] = {
        .eType = kSettingDataType_U8,
        .U8 = 1,
    },
    [kSettingKey_ColorCorrectLCD] = {
        .eType = kSettingDataType_U8,
        .U8 = 0,
    },
    [kSettingKey_ColorCorrectUSB] = {
        .eType = kSettingDataType_U8,
        .U8 = 1,
    },
    [kSettingKey_PlayerNum] = {
        .eType = kSettingDataType_U8,
        .U8 = 1,
    },
    [kSettingKey_SilentMode] = {
        .eType = kSettingDataType_U8,
        .U8 = 0,
    },
    [kSettingKey_Brightness] = {
        .eType = kSettingDataType_U8,
        .U8 = 7,
    },
    [kSettingKey_ScreenTransitCtl] = {
        .eType = kSettingDataType_U8,
        .U8 = 0,
    },
    [kSettingKey_DPadCtl] = {
        .eType = kSettingDataType_U8,
        .U8 = 0,
    },
    [kSettingKey_LowBattIconCtl] = {
        .eType = kSettingDataType_U8,
        .U8 = 0,
    }
};

static const char* TAG = "Settings";
static nvs_handle_t _Handle = kNoHandle;

OSD_Result_t Settings_Initialize(void)
{
    if (_Handle != kNoHandle)
    {
        return kOSD_Result_Err_SettingsNVSAlreadyLoaded;
    }

    esp_err_t esp_err = nvs_flash_init();

    if ((esp_err == ESP_ERR_NVS_NO_FREE_PAGES) || (esp_err == ESP_ERR_NVS_NEW_VERSION_FOUND))
    {
        ESP_LOGW(TAG, "NVS parition changed (%s). Reformatting (all settings are lost!).", esp_err_to_name(esp_err));

        ESP_ERROR_CHECK(nvs_flash_erase());
        esp_err = nvs_flash_init();
    }

    ESP_ERROR_CHECK( esp_err );

    esp_err = nvs_open(Settings_GetNVSNamespace(), NVS_READWRITE, &_Handle);
    if (esp_err != ESP_OK)
    {
        ESP_LOGW(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(esp_err));
    }

    return kOSD_Result_Ok;
}

OSD_Result_t Settings_Update(const SettingKey_t eKey, const uint32_t Value)
{
    if ((unsigned)eKey >= kNumSettingKeys)
    {
        return kOSD_Result_Err_SettingKeyInvalid;
    }

    if (_Handle == kNoHandle)
    {
        return kOSD_Result_Err_SettingsNVSNotLoaded;
    }

    if (NVS_KeyStr[eKey] == NULL)
    {
        ESP_LOGE(TAG, "undefined key name for %d", eKey);
        return kOSD_Result_Err_NullDataPtr;
    }

    esp_err_t esp_err = ESP_OK;

    // TODO Support other data types
    switch (DefaultSettings[eKey].eType)
    {
        case kSettingDataType_U8:
            esp_err = nvs_set_u8(_Handle, NVS_KeyStr[eKey], (uint8_t)(Value & 0xFF));
            break;
        default:
            return kOSD_Result_Err_SettingInvalidWidth;
    }

    if (esp_err != ESP_OK)
    {
        ESP_LOGE(TAG, "Update failed for %s: %s", NVS_KeyStr[eKey], esp_err_to_name(esp_err));
        return kOSD_Result_Err_SettingUpdateFailed;
    }

    return kOSD_Result_Ok;
}

OSD_Result_t Settings_Retrieve(const SettingKey_t eKey, uint32_t *const pValue)
{
    if ((unsigned)eKey >= kNumSettingKeys)
    {
        return kOSD_Result_Err_SettingKeyInvalid;
    }

    if (pValue == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    if (NVS_KeyStr[eKey] == NULL)
    {
        ESP_LOGE(TAG, "undefined key name for %d", eKey);
        return kOSD_Result_Err_NullDataPtr;
    }

    if (_Handle == kNoHandle)
    {
        return kOSD_Result_Err_SettingsNVSNotLoaded;
    }

    esp_err_t esp_err = ESP_OK;

    // TODO Support other data types
    switch (DefaultSettings[eKey].eType)
    {
        case kSettingDataType_U8:
            uint8_t ReadValue;
            esp_err = nvs_get_u8(_Handle, NVS_KeyStr[eKey], &ReadValue);

            if (esp_err == ESP_OK)
            {
                *pValue = (uint32_t)ReadValue;
            }
            break;
        default:
            return kOSD_Result_Err_SettingInvalidWidth;
    }

    if (esp_err != ESP_OK)
    {
        ESP_LOGE(TAG, "Read failed for %s: %s", NVS_KeyStr[eKey], esp_err_to_name(esp_err));
        return kOSD_Result_Err_SettingReadFailed;
    }

    return kOSD_Result_Ok;
}

const char* Settings_GetNVSNamespace(void)
{
    return "storage";
}

const char* Settings_KeyToName(const SettingKey_t eKey)
{
    if ((unsigned)eKey >= kNumSettingKeys)
    {
        return NULL;
    }

    if (NVS_KeyStr[eKey] == NULL)
    {
        ESP_LOGE(TAG, "undefined name for key %d", eKey);
        return NULL;
    }

    return NVS_KeyStr[eKey];
}

OSD_Result_t Settings_RetrieveDefault(const SettingKey_t eKey, SettingValue_t *const pValue)
{
    if ((unsigned)eKey >= kNumSettingKeys)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    if (pValue == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    *pValue = DefaultSettings[eKey];

    return kOSD_Result_Ok;
}
