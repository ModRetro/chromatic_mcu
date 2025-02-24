#include "silent.h"

#include "esp_log.h"
#include "osd_shared.h"
#include "lvgl.h"
#include "mutex.h"
#include "settings.h"

LV_IMG_DECLARE(jk_dot_k14);
LV_IMG_DECLARE(img_a_right);
LV_IMG_DECLARE(img_b_left);

enum {
    kMinPlayerNum = 1,
    kMaxPlayerNum = 8,

    kMsgStateX_px = 105,
    kMsgStateY_px = 63,

    kImgBLeftStateX_px = 80,
    kImgBLeftStateY_px = 63,

    kImgARightStateX_px = 131,
    kImgARightStateY_px = 63,
};

typedef struct PlayerNum {
    lv_obj_t* pMsgStateObj;
    lv_obj_t* pImgARightObj;
    lv_obj_t* pImgBLeftObj;
    uint8_t Number;
    fnOnUpdateCb_t fnOnUpdateCb;
} PlayerNum_t;

static const char* TAG = "PlayerNum";
static PlayerNum_t _Ctx = {
    .Number = kMinPlayerNum,
};

static void SaveToSettings(const uint8_t PlayerNum);

OSD_Result_t PlayerNum_Draw(void* arg)
{
    if (arg == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    lv_obj_t *const pScreen = (lv_obj_t *const) arg;

    if (_Ctx.pMsgStateObj == NULL)
    {
        _Ctx.pMsgStateObj = lv_label_create(pScreen);

        lv_obj_add_style(_Ctx.pMsgStateObj, OSD_GetStyleTextWhite_L(), 0);
        lv_obj_align(_Ctx.pMsgStateObj, LV_ALIGN_TOP_LEFT, kMsgStateX_px, kMsgStateY_px);
    }

    if (_Ctx.pImgBLeftObj == NULL)
    {
        _Ctx.pImgBLeftObj = lv_img_create(pScreen);
        lv_obj_align(_Ctx.pImgBLeftObj, LV_ALIGN_TOP_LEFT, kImgBLeftStateX_px, kImgBLeftStateY_px);
        lv_img_set_src(_Ctx.pImgBLeftObj, &img_b_left);
    }

    if (_Ctx.pImgARightObj == NULL)
    {
        _Ctx.pImgARightObj = lv_img_create(pScreen);
        lv_obj_align(_Ctx.pImgARightObj, LV_ALIGN_TOP_LEFT, kImgARightStateX_px, kImgARightStateY_px);
        lv_img_set_src(_Ctx.pImgARightObj, &img_a_right);
    }

    lv_label_set_text_fmt(_Ctx.pMsgStateObj, "%u", _Ctx.Number);
    lv_obj_move_foreground(_Ctx.pMsgStateObj);
    lv_obj_move_foreground(_Ctx.pImgARightObj);
    lv_obj_move_foreground(_Ctx.pImgBLeftObj);

    return kOSD_Result_Ok;
}

void PlayerNum_Update(const uint8_t NewNum)
{
    if ((NewNum < kMinPlayerNum) || (NewNum > kMaxPlayerNum))
    {
        return;
    }

    if (Mutex_Take(kMutexKey_PlayerNum) == kMutexResult_Ok)
    {
        const uint8_t Number = _Ctx.Number;
        _Ctx.Number = NewNum;

        if (NewNum != Number)
        {
            SaveToSettings(NewNum);
        }

        (void) Mutex_Give(kMutexKey_PlayerNum);
    }

    if (_Ctx.fnOnUpdateCb != NULL)
    {
        _Ctx.fnOnUpdateCb();
    }
}

OSD_Result_t PlayerNum_OnButton(const Button_t Button, const ButtonState_t State, void *arg)
{
    (void)arg;

    switch (Button)
    {
        case kButton_A:
            if (State == kButtonState_Pressed)
            {
                PlayerNum_Update(_Ctx.Number + 1);
            }
            break;
        case kButton_B:
            if (State == kButtonState_Pressed)
            {
                PlayerNum_Update(_Ctx.Number - 1);
            }
            break;
        default:
            break;
    }
    return kOSD_Result_Ok;
}

OSD_Result_t PlayerNum_OnTransition(void* arg)
{
    (void)arg;

    lv_obj_t** ToDelete[] = {
        &_Ctx.pMsgStateObj,
        &_Ctx.pImgARightObj,
        &_Ctx.pImgBLeftObj,
    };

    for (size_t i = 0; i < ARRAY_SIZE(ToDelete); i++)
    {
        if (*ToDelete[i] != NULL)
        {
            lv_obj_del(*ToDelete[i]);
            *(ToDelete[i]) = NULL;
        }
    }

    return kOSD_Result_Ok;
}

uint8_t PlayerNum_GetNum(void)
{
    return _Ctx.Number;
}

OSD_Result_t PlayerNum_ApplySetting(SettingValue_t const *const pValue)
{
    if (pValue == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    if ((pValue->eType == kSettingDataType_U8) && (pValue->U8 >= kMinPlayerNum) && (pValue->U8 <= kMaxPlayerNum))
    {
        _Ctx.Number = pValue->U8;
    }
    else
    {
        return kOSD_Result_Err_UnexpectedSettingDataType;
    }

    return kOSD_Result_Ok;
}

void PlayerNum_RegisterOnUpdateCb(fnOnUpdateCb_t fnOnUpdate)
{
    _Ctx.fnOnUpdateCb = fnOnUpdate;
}

static void SaveToSettings(const uint8_t PlayerNum)
{
    OSD_Result_t eResult;
    if ((eResult = Settings_Update(kSettingKey_PlayerNum, PlayerNum)) != kOSD_Result_Ok)
    {
        ESP_LOGE(TAG, "Player num save failed: %d", eResult);
    }
}
