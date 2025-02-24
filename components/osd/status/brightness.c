#include "brightness.h"

#include "line.h"
#include "osd_shared.h"
#include "esp_log.h"
#include "lvgl.h"
#include "mutex.h"
#include "settings.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//#define BRIGHTNESS_DISP_TEST

enum {
    kLevelOffsetX_px = 81,
    kLevelOffsetY_px = 57,
    kLevelEndPosY_px = kLevelOffsetY_px + 26,

    kBarWidth_px = 7,
    kBarGap_px   = 1,

    kColorFilled = 0xFFFFFF,

    kNumBars = 8,

    // Aligns with 15 possible brightness levels controlled by the FPGA
    kMaxBrightness = kNumBars * 2,

    // For 15 possible values, the minimum brightness must show one bar.
    kMinBrightness = 1,

    kNoPartialFound = -1,
};

LV_IMG_DECLARE(img_brightness);

static const char* TAG = "bright";

static Line_t _bars[kNumBars] = {
    { .points = { {0, 0}, {0, 0} }, .width = kBarWidth_px, /* color is set by level*/ },
    { .points = { {0, 0}, {0, 0} }, .width = kBarWidth_px, /* color is set by level*/ },
    { .points = { {0, 0}, {0, 0} }, .width = kBarWidth_px, /* color is set by level*/ },
    { .points = { {0, 0}, {0, 0} }, .width = kBarWidth_px, /* color is set by level*/ },
    { .points = { {0, 0}, {0, 0} }, .width = kBarWidth_px, /* color is set by level*/ },
    { .points = { {0, 0}, {0, 0} }, .width = kBarWidth_px, /* color is set by level*/ },
    { .points = { {0, 0}, {0, 0} }, .width = kBarWidth_px, /* color is set by level*/ },
    { .points = { {0, 0}, {0, 0} }, .width = kBarWidth_px, /* color is set by level*/ },
};

typedef struct Brightness {
    lv_obj_t* pBrightImgObj;
    int8_t BrightnessLevel;
    bool LowPowerOverride;
    lv_obj_t* pLowPwrWarnObj;

    #ifdef BRIGHTNESS_DISP_TEST
    int32_t TestMode;
    #endif

    fnOnUpdateCb_t fnOnUpdateCb;
} Brightness_t;

static Brightness_t _Ctx = {
    #ifdef BRIGHTNESS_DISP_TEST
    .TestMode = -1,
    #endif
};

static void SaveToSettings(const uint8_t Brightness);
static void ClearBrightnessBars(void);

OSD_Result_t Brightness_Draw(void* arg)
{
    if (arg == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    lv_obj_t *const pScreen = (lv_obj_t *const) arg;

    if (_Ctx.pBrightImgObj == NULL)
    {
        _Ctx.pBrightImgObj = lv_img_create(pScreen);
        lv_obj_align(_Ctx.pBrightImgObj, LV_ALIGN_CENTER, 29, -2);
    }

    // Draw the brightness image indicating no brightness
    lv_img_set_src(_Ctx.pBrightImgObj, &img_brightness);

    static uint8_t PrevBrightness = 0;
    const bool RecalculateBars = (_Ctx.BrightnessLevel != PrevBrightness);
    PrevBrightness = _Ctx.BrightnessLevel;

    if (_Ctx.LowPowerOverride)
    {
        ClearBrightnessBars();

        if (_Ctx.pLowPwrWarnObj == NULL)
        {
            _Ctx.pLowPwrWarnObj = lv_label_create(pScreen);
            lv_obj_add_style(_Ctx.pLowPwrWarnObj, OSD_GetStyleTextWhite(), 0);
            lv_obj_set_align(_Ctx.pLowPwrWarnObj, LV_ALIGN_TOP_LEFT);
            lv_obj_set_pos(_Ctx.pLowPwrWarnObj, kLevelOffsetX_px - 5, kLevelEndPosY_px + 5);

            lv_label_set_text_static(_Ctx.pLowPwrWarnObj, "POWER SAVE MODE ON,\nREPLACE BATTERIES.");
        }

        return kOSD_Result_Ok;
    }
    else
    {
        if (_Ctx.pLowPwrWarnObj != NULL)
        {
            lv_obj_del(_Ctx.pLowPwrWarnObj);
            _Ctx.pLowPwrWarnObj = NULL;
        }
    }

    // Overlay any solid bars based on the actual level
    const size_t BrightBars = MIN(_Ctx.BrightnessLevel, kMaxBrightness);

    // An odd brightness level means the final bar is partially filled
    int32_t PartialBarNum = kNoPartialFound;
    if ((BrightBars % 2) != 0)
    {
        if (BrightBars == 1)
        {
            PartialBarNum = 1;
        }
        else
        {
            PartialBarNum = (int32_t)(BrightBars)/2 + 1;
        }
    }

    const uint32_t FullBarCount = BrightBars / 2;

    // Fill in all full bars
    for (size_t i=0; i<kNumBars; i++)
    {
        if (RecalculateBars)
        {
            // The brightness could have been modified by a hotkey. Start over
            if (_bars[i].pObj != NULL)
            {
                lv_obj_del(_bars[i].pObj);
                _bars[i].pObj = NULL;
            }
        }

        if (_bars[i].pObj == NULL)
        {
            // Cache the line object the very first time. The line needs to persist for the duration of the draw event
            _bars[i].pObj = lv_line_create(pScreen);
        }
    }

    for (size_t i=0; i<FullBarCount; i++)
    {
        if (RecalculateBars)
        {
            const lv_point_t FullBarPoints[kLineConst_NumPoints] = {
                {.x = kLevelOffsetX_px, .y = kLevelOffsetY_px},
                {.x = kLevelOffsetX_px, .y = kLevelEndPosY_px },
            };

            memcpy(_bars[i].points, FullBarPoints, sizeof(FullBarPoints));

            _bars[i].points[kLineConst_PointStartIdx].x += i * ( kBarWidth_px + kBarGap_px);
            _bars[i].points[kLineConst_PointEndIdx].x   += i * ( kBarWidth_px + kBarGap_px);
        }

        lv_line_set_points(_bars[i].pObj, _bars[i].points, kLineConst_NumPoints);
        lv_obj_set_style_line_color(_bars[i].pObj, lv_color_hex(kColorFilled), LV_PART_MAIN);
        lv_obj_set_style_line_width(_bars[i].pObj, _bars[i].width, LV_PART_MAIN);
    }

    // Fill in the final partial bar
    if ((PartialBarNum != 0) && (PartialBarNum != kNoPartialFound))
    {
        // All bars are zero-indexed
        PartialBarNum--;

        const lv_point_t PartialBarPoints[kLineConst_NumPoints] = {
            {.x = 81, .y = 57},
            {.x = 81, .y = 71},
        };

        if (RecalculateBars)
        {
            memcpy(_bars[PartialBarNum].points, PartialBarPoints, sizeof(PartialBarPoints));

            _bars[PartialBarNum].points[kLineConst_PointStartIdx].x += PartialBarNum * ( kBarWidth_px + kBarGap_px);
            _bars[PartialBarNum].points[kLineConst_PointEndIdx].x   += PartialBarNum * ( kBarWidth_px + kBarGap_px);
        }

        lv_line_set_points(_bars[PartialBarNum].pObj, _bars[PartialBarNum].points, kLineConst_NumPoints);
        lv_obj_set_style_line_color(_bars[PartialBarNum].pObj, lv_color_hex(kColorFilled), LV_PART_MAIN);
        lv_obj_set_style_line_width(_bars[PartialBarNum].pObj, _bars[PartialBarNum].width, LV_PART_MAIN);
    }
    else if ((BrightBars < kMaxBrightness) && (BrightBars % 2 == 0))
    {
        size_t PartialBarToDel = FullBarCount;
        lv_obj_set_style_line_width(_bars[PartialBarToDel].pObj, 0, LV_PART_MAIN);
    }

    return kOSD_Result_Ok;
}

uint8_t Brightness_GetLevel(void)
{
    if (_Ctx.BrightnessLevel == 0)
    {
        return 0;
    }
    else if (_Ctx.LowPowerOverride)
    {
        return kMinBrightness - 1;
    }
    else
    {
        return _Ctx.BrightnessLevel - 1;
    }
}

void Brightness_Update(const uint8_t NewBrightness)
{
    #ifdef BRIGHTNESS_DISP_TEST
    if (_Ctx.TestMode != 0)
    {
        if (_Ctx.TestMode == 1)
        {
            _Ctx.BrightnessLevel--;
            if (_Ctx.BrightnessLevel <= 0) {
                _Ctx.TestMode = -1;
            }
        }
        else
        {
            _Ctx.BrightnessLevel++;
            if (_Ctx.BrightnessLevel >= kMaxBrightness) {
                _Ctx.TestMode = 1;
            }
        }
    }
    else
    #endif
    if (_Ctx.LowPowerOverride)
    {
        return;
    }
    else if ((NewBrightness > 0) && (NewBrightness <= kMaxBrightness))
    {
        if (Mutex_Take(kMutexKey_Brightness) == kMutexResult_Ok)
        {
            const int8_t CurrentBrightness = _Ctx.BrightnessLevel;

            if (CurrentBrightness != NewBrightness)
            {
                SaveToSettings(NewBrightness);
            }

            _Ctx.BrightnessLevel = NewBrightness;

            (void) Mutex_Give(kMutexKey_Brightness);
        }

        if (_Ctx.fnOnUpdateCb != NULL)
        {
            _Ctx.fnOnUpdateCb();
        }
    }
}

OSD_Result_t Brightness_OnTransition(void* arg)
{
    (void)(arg);

    if (_Ctx.pBrightImgObj != NULL)
    {
        lv_obj_del(_Ctx.pBrightImgObj);
        _Ctx.pBrightImgObj = NULL;
    }

    if (_Ctx.pLowPwrWarnObj != NULL)
    {
        lv_obj_del(_Ctx.pLowPwrWarnObj);
        _Ctx.pLowPwrWarnObj = NULL;
    }

    ClearBrightnessBars();

    return kOSD_Result_Ok;
}

OSD_Result_t Brightness_OnButton(const Button_t Button, const ButtonState_t State, void *arg)
{
    (void)arg;

    switch (Button)
    {
        case kButton_B:
            if (State == kButtonState_Pressed)
            {
                Brightness_Update(_Ctx.BrightnessLevel - 1);
            }
            break;
        case kButton_A:
            if (State == kButtonState_Pressed)
            {
                Brightness_Update(_Ctx.BrightnessLevel + 1);
            }
            break;
        default:
            break;
    }
    return kOSD_Result_Ok;
}

OSD_Result_t Brightness_ApplySetting(SettingValue_t const *const pValue)
{
    if (pValue == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    if (pValue->eType == kSettingDataType_U8)
    {
        _Ctx.BrightnessLevel = MIN(pValue->U8, kMaxBrightness);
        _Ctx.BrightnessLevel = MAX(_Ctx.BrightnessLevel, kMinBrightness);
    }
    else
    {
        return kOSD_Result_Err_UnexpectedSettingDataType;
    }

    return kOSD_Result_Ok;
}

void Brightness_RegisterOnUpdateCb(fnOnUpdateCb_t fnOnUpdate)
{
    _Ctx.fnOnUpdateCb = fnOnUpdate;
}

void Brightness_SetLowPowerOverride(const bool Enable)
{
    _Ctx.LowPowerOverride = Enable;

    if (_Ctx.fnOnUpdateCb != NULL)
    {
        _Ctx.fnOnUpdateCb();
    }
}

static void SaveToSettings(const uint8_t Brightness)
{
    OSD_Result_t eResult;
    if ((eResult = Settings_Update(kSettingKey_Brightness, Brightness)) != kOSD_Result_Ok)
    {
        ESP_LOGE(TAG, "Brightness save failed: %d", eResult);
    }
}

static void ClearBrightnessBars(void)
{
    // Delete all brightness bar objects
    for (size_t i=0; i<kNumBars; i++)
    {
        if (_bars[i].pObj != NULL)
        {
            lv_obj_del(_bars[i].pObj);
            _bars[i].pObj = NULL;
        }
    }
}
