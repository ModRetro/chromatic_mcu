#include "dpad_ctl.h"
#include "line.h"

#include "esp_log.h"
#include "osd_shared.h"
#include "lvgl.h"
#include "mutex.h"
#include "settings.h"

LV_IMG_DECLARE(img_option_en);
LV_IMG_DECLARE(img_option_dis);

enum {
    kToggleOptX_px = 77,

    kToggleOptTop_Y_px = 52,
    kToggleOptBtm_Y_px = 76,
};

typedef struct DPadCtl {
    lv_obj_t* pImgOptionTopObj;
    lv_obj_t* pImgOptionBtmObj;
    lv_obj_t* pOptionTopTextObj;
    lv_obj_t* pOptionBtmTextObj;
    lv_obj_t* pDPadRectVert;
    lv_obj_t* pDPadRectHoriz;
    DPadCtlState_t eCurrentState;
    fnOnUpdateCb_t fnOnUpdateCb;
    bool FillInVisible;
} DPadCtl_t;

static const char* TAG = "DPad";
static DPadCtl_t _Ctx;

static void SaveToSettings(const DPadCtlState_t eNewState);
static const uint32_t UpdatePeriod_ms = 30;
static uint32_t UpdateCounter = 0;
static const uint32_t BlinkRate = (240 / UpdatePeriod_ms);

enum {
    kVert,
    kHoriz,
    kNumDPadCrosses,

    kColor_White = 0xFFFFFF,
};

static Line_t _DPadFill[kNumDPadCrosses] = {
    [kVert]  = { .points = { {37, 80}, {37, 89} }, .width = 3,  kColor_White },
    [kHoriz] = { .points = { {33, 84}, {42, 84} }, .width = 3, kColor_White },
};

OSD_Result_t DPadCtl_Draw(void* arg)
{
    if (arg == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    lv_obj_t *const pScreen = (lv_obj_t *const) arg;

    const bool IsDisabled = (_Ctx.eCurrentState != kDPadCtlState_AcceptDiag);

    if (++UpdateCounter >= BlinkRate)
    {
        UpdateCounter = 0;
        _Ctx.FillInVisible ^= true;
    }

    if (_Ctx.FillInVisible)
    {
        for (size_t i=0; i<ARRAY_SIZE(_DPadFill); i++)
        {
            if (_DPadFill[i].pObj == NULL)
            {
                // Cache the line object the very first time. The line needs to persist for the duration of the draw event
                _DPadFill[i].pObj = lv_line_create(pScreen);
                lv_obj_remove_style_all(_DPadFill[i].pObj);
                lv_obj_set_style_line_width(_DPadFill[i].pObj, _DPadFill[i].width, LV_PART_MAIN);
                lv_obj_set_style_line_color(_DPadFill[i].pObj, lv_color_hex(_DPadFill[i].color), LV_PART_MAIN);
                lv_line_set_points(_DPadFill[i].pObj, _DPadFill[i].points, kLineConst_NumPoints);
                lv_obj_set_style_line_rounded(_DPadFill[i].pObj, false, LV_PART_MAIN);
            }
        }
    }
    else
    {
        for (size_t i=0; i<ARRAY_SIZE(_DPadFill); i++)
        {
            if (_DPadFill[i].pObj != NULL)
            {
                lv_obj_del(_DPadFill[i].pObj);
                _DPadFill[i].pObj = NULL;
            }
        }
    }

    if (_Ctx.pImgOptionTopObj == NULL)
    {
        _Ctx.pImgOptionTopObj = lv_img_create(pScreen);

        lv_obj_t* pOff = _Ctx.pImgOptionTopObj;
        lv_obj_align(pOff, LV_ALIGN_TOP_LEFT, kToggleOptX_px, IsDisabled ? kToggleOptTop_Y_px : kToggleOptBtm_Y_px);
        lv_img_set_src(pOff, &img_option_dis);
    }

    if (_Ctx.pImgOptionBtmObj == NULL)
    {
        _Ctx.pImgOptionBtmObj = lv_img_create(pScreen);

        lv_obj_t* pOn = _Ctx.pImgOptionBtmObj;
        lv_obj_align(pOn, LV_ALIGN_TOP_LEFT, kToggleOptX_px, IsDisabled ? kToggleOptBtm_Y_px : kToggleOptTop_Y_px);
        lv_img_set_src(pOn, &img_option_en);
    }

    if (_Ctx.pOptionTopTextObj == NULL)
    {
        _Ctx.pOptionTopTextObj = lv_label_create(IsDisabled ? _Ctx.pImgOptionTopObj : _Ctx.pImgOptionBtmObj);
        lv_obj_align(_Ctx.pOptionTopTextObj, LV_ALIGN_CENTER, 1, 1);
        lv_label_set_long_mode(_Ctx.pOptionTopTextObj, LV_LABEL_LONG_WRAP);
        lv_obj_set_style_text_align(_Ctx.pOptionTopTextObj, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(_Ctx.pOptionTopTextObj, img_option_en.header.w * 0.75);
        lv_obj_add_style(_Ctx.pOptionTopTextObj, OSD_GetStyleTextBlack(), 0);
        lv_label_set_text(_Ctx.pOptionTopTextObj, "ALLOW ALL DIRECTIONS");
    }

    if (_Ctx.pOptionBtmTextObj == NULL)
    {
        _Ctx.pOptionBtmTextObj = lv_label_create(IsDisabled ? _Ctx.pImgOptionBtmObj : _Ctx.pImgOptionTopObj);
        lv_obj_align(_Ctx.pOptionBtmTextObj, LV_ALIGN_CENTER, 1, 1);
        lv_label_set_long_mode(_Ctx.pOptionBtmTextObj, LV_LABEL_LONG_WRAP);
        lv_obj_set_style_text_align(_Ctx.pOptionBtmTextObj, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(_Ctx.pOptionBtmTextObj, img_option_en.header.w * 0.80);
        lv_obj_add_style(_Ctx.pOptionBtmTextObj, OSD_GetStyleTextBlack(), 0);
        lv_label_set_text(_Ctx.pOptionBtmTextObj, "IGNORE DIAGONALS");
    }

    return kOSD_Result_Ok;
}

void DPadCtl_Update(const DPadCtlState_t eNewState)
{
    if ((unsigned)eNewState >= kNumDPadCtlState)
    {
        return;
    }

    DPadCtl_OnTransition(NULL);

    if (Mutex_Take(kMutexKey_DPadCtl) == kMutexResult_Ok)
    {
        const DPadCtlState_t eCurrentState = _Ctx.eCurrentState;
        _Ctx.eCurrentState = eNewState;

        if (eNewState != eCurrentState)
        {
            SaveToSettings(eNewState);
        }

        (void) Mutex_Give(kMutexKey_DPadCtl);
    }

    if (_Ctx.fnOnUpdateCb != NULL)
    {
        _Ctx.fnOnUpdateCb();
    }
}

OSD_Result_t DPadCtl_OnButton(const Button_t Button, const ButtonState_t State, void *arg)
{
    (void)arg;

    switch (Button)
    {
        case kButton_A:
        {
            if (State == kButtonState_Pressed)
            {
                const DPadCtlState_t eState = _Ctx.eCurrentState;
                ESP_LOGI(TAG, "Updating D-Pad directional control from %d", eState);

                DPadCtl_Update(eState ^ 1);
            }
            break;
        }
        default:
            break;
    }
    return kOSD_Result_Ok;
}

OSD_Result_t DPadCtl_OnTransition(void* arg)
{
    (void)arg;

    lv_obj_t** ToDelete[] = {
        &_Ctx.pOptionTopTextObj,
        &_Ctx.pOptionBtmTextObj,
        &_Ctx.pImgOptionTopObj,
        &_Ctx.pImgOptionBtmObj,
        &_DPadFill[kVert].pObj,
        &_DPadFill[kHoriz].pObj,
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

DPadCtlState_t DPadCtl_GetState(void)
{
    return _Ctx.eCurrentState;
}

OSD_Result_t DPadCtl_ApplySetting(SettingValue_t const *const pValue)
{
    if (pValue == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    if (pValue->eType == kSettingDataType_U8)
    {
        const DPadCtlState_t eState = (pValue->U8 == 0) ? kDPadCtlState_AcceptDiag : kDPadCtlState_RejectDiag;
        _Ctx.eCurrentState = eState;
    }
    else
    {
        return kOSD_Result_Err_UnexpectedSettingDataType;
    }

    return kOSD_Result_Ok;
}

void DPadCtl_RegisterOnUpdateCb(fnOnUpdateCb_t fnOnUpdate)
{
    _Ctx.fnOnUpdateCb = fnOnUpdate;
}

static void SaveToSettings(const DPadCtlState_t eNewState)
{
#if defined(ESP_PLATFORM)
    OSD_Result_t eResult;
    if ((eResult = Settings_Update(kSettingKey_DPadCtl, eNewState)) != kOSD_Result_Ok)
    {
        ESP_LOGE(TAG, "Screen transition control save failed: %d", eResult);
    }
#endif
}
