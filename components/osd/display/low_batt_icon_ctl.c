#include "low_batt_icon_ctl.h"

#include "esp_log.h"
#include "osd_shared.h"
#include "lvgl.h"
#include "mutex.h"
#include "settings.h"

LV_IMG_DECLARE(img_option_dis);
LV_IMG_DECLARE(img_option_en);

enum {
    kToggleOptX_px = 77,

    kToggleOptShow_Y_px = 42,
    kToggleOptFlash_Y_px = 64,
    kToggleOptHide_Y_px = 86,
};

typedef struct LowBattIconCtl {
    lv_obj_t* pImgOptionObjs[kNumLowBattIconCtlState];
    lv_obj_t* pOptionTextObj[kNumLowBattIconCtlState];
    LowBattIconCtlState_t eCurrentState;
    fnOnUpdateCb_t fnOnUpdateCb;
} LowBattIconCtl_t;

lv_point_t OptionPos[kNumLowBattIconCtlState] = {
    [kLowBattIconCtlState_AlwaysShow] = {.x = kToggleOptX_px, .y = kToggleOptShow_Y_px},
    [kLowBattIconCtlState_Flash]      = {.x = kToggleOptX_px, .y = kToggleOptFlash_Y_px},
    [kLowBattIconCtlState_Hide]       = {.x = kToggleOptX_px, .y = kToggleOptHide_Y_px},
};

static const char* OptionText[kNumLowBattIconCtlState] = {
    [kLowBattIconCtlState_AlwaysShow] = "ALWAYS SHOW",
    [kLowBattIconCtlState_Flash]      = "BLINK",
    [kLowBattIconCtlState_Hide]       = "HIDE",
};

static const char* TAG = "LBICtl";
static LowBattIconCtl_t _Ctx ;

static void SaveToSettings(const LowBattIconCtlState_t eNewState);

OSD_Result_t LowBattIconCtl_Draw(void* arg)
{
    if (arg == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    lv_obj_t *const pScreen = (lv_obj_t *const) arg;

    for (uint32_t i = 0; i < kNumLowBattIconCtlState; i++)
    {
        if (_Ctx.pImgOptionObjs[i] == NULL)
        {
            _Ctx.pImgOptionObjs[i] = lv_img_create(pScreen);

            lv_obj_align(_Ctx.pImgOptionObjs[i], LV_ALIGN_TOP_LEFT, OptionPos[i].x, OptionPos[i].y);
            lv_img_set_src(_Ctx.pImgOptionObjs[i], &img_option_dis);
        }
    }

    if ((unsigned) _Ctx.eCurrentState < kNumLowBattIconCtlState)
    {
        lv_img_set_src(_Ctx.pImgOptionObjs[_Ctx.eCurrentState], &img_option_en);
    }

    for (uint32_t i = 0; i < kNumLowBattIconCtlState; i++)
    {
        if (_Ctx.pOptionTextObj[i] == NULL)
        {
            _Ctx.pOptionTextObj[i] = lv_label_create(_Ctx.pImgOptionObjs[i]);
            lv_obj_align(_Ctx.pOptionTextObj[i], LV_ALIGN_CENTER, 1, 0);
            lv_label_set_long_mode(_Ctx.pOptionTextObj[i], LV_LABEL_LONG_WRAP);
            lv_obj_set_style_text_align(_Ctx.pOptionTextObj[i], LV_TEXT_ALIGN_CENTER, 0);
            lv_obj_set_width(_Ctx.pOptionTextObj[i], img_option_en.header.w * 0.80);
            lv_obj_add_style(_Ctx.pOptionTextObj[i], OSD_GetStyleTextBlack(), 0);
            lv_label_set_text(_Ctx.pOptionTextObj[i], OptionText[i]);
        }
    }

    return kOSD_Result_Ok;
}

void LowBattIconCtl_Update(const LowBattIconCtlState_t eNewState)
{
    if ((unsigned)eNewState >= kNumLowBattIconCtlState)
    {
        return;
    }

    LowBattIconCtl_OnTransition(NULL);

    if (Mutex_Take(kMutexKey_LowBattIconCtl) == kMutexResult_Ok)
    {
        const LowBattIconCtlState_t eCurrentState = _Ctx.eCurrentState;
        _Ctx.eCurrentState = eNewState;

        if (eNewState != eCurrentState)
        {
            SaveToSettings(eNewState);
        }

        (void) Mutex_Give(kMutexKey_LowBattIconCtl);
    }

    if (_Ctx.fnOnUpdateCb != NULL)
    {
        _Ctx.fnOnUpdateCb();
    }
}

OSD_Result_t LowBattIconCtl_OnButton(const Button_t Button, const ButtonState_t State, void *arg)
{
    (void)arg;

    switch (Button)
    {
        case kButton_A:
        {
            if (State == kButtonState_Pressed)
            {
                LowBattIconCtlState_t NewState = _Ctx.eCurrentState + 1;
                NewState %= kNumLowBattIconCtlState;

                LowBattIconCtl_Update(NewState);
            }
            break;
        }
        case kButton_B:
        {
            if (State == kButtonState_Pressed)
            {
                LowBattIconCtlState_t NewState = _Ctx.eCurrentState;
                if (NewState > 0)
                {
                    NewState--;
                }
                else
                {
                    NewState = kNumLowBattIconCtlState - 1;
                }

                LowBattIconCtl_Update(NewState);
            }
            break;
        }
        default:
            break;
    }
    return kOSD_Result_Ok;
}

OSD_Result_t LowBattIconCtl_OnTransition(void* arg)
{
    (void)arg;

    for (uint32_t i = 0; i < kNumLowBattIconCtlState; i++)
    {
        if (_Ctx.pOptionTextObj[i] != NULL)
        {
            lv_obj_del(_Ctx.pOptionTextObj[i]);
            _Ctx.pOptionTextObj[i] = NULL;
        }

        if (_Ctx.pImgOptionObjs[i] != NULL)
        {
            lv_obj_del(_Ctx.pImgOptionObjs[i]);
            _Ctx.pImgOptionObjs[i] = NULL;
        }
    }

    return kOSD_Result_Ok;
}

LowBattIconCtlState_t LowBattIconCtl_GetState(void)
{
    return _Ctx.eCurrentState;
}

OSD_Result_t LowBattIconCtl_ApplySetting(SettingValue_t const *const pValue)
{
    if (pValue == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    if (pValue->eType == kSettingDataType_U8)
    {
        LowBattIconCtlState_t eState = (LowBattIconCtlState_t)(pValue->U8);

        if ((unsigned)eState >= kNumLowBattIconCtlState)
        {
            eState = kLowBattIconCtlState_AlwaysShow;
        }

        _Ctx.eCurrentState = eState;
    }
    else
    {
        return kOSD_Result_Err_UnexpectedSettingDataType;
    }

    return kOSD_Result_Ok;
}

void LowBattIconCtl_RegisterOnUpdateCb(fnOnUpdateCb_t fnOnUpdate)
{
    _Ctx.fnOnUpdateCb = fnOnUpdate;
}

static void SaveToSettings(const LowBattIconCtlState_t eNewState)
{
    ESP_LOGD(TAG, "Saving %d to low battery icon control", eNewState);

    OSD_Result_t eResult;
    if ((eResult = Settings_Update(kSettingKey_LowBattIconCtl, eNewState)) != kOSD_Result_Ok)
    {
        ESP_LOGE(TAG, "Low battery icon save failed: %d", eResult);
    }
}
