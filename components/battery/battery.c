#include "battery.h"

#include "osd_shared.h"
#include "line.h"
#include "lvgl.h"
#include "mutex.h"
#include "esp_log.h"

#if defined(ESP_PLATFORM)
#include "freertos/FreeRTOS.h"
#endif

#include <stdint.h>
#include <stdbool.h>

// FPGA sends battery data, on average, every second
#define SAMPLING_INTERVAL_S     1.3f
#define SAMPLING_INTERVAL_HZ    ( 1.0f / SAMPLING_INTERVAL_S)

// Choice is to yield a smoother bu eliminate most noise
#define CUTOFF_FREQ_HZ          0.05f

#if !defined(ESP_PLATFORM)
typedef uint32_t TickType_t;
#endif

// #define BATT_DISP_TEST
// #define BATT_DISP_VOLTAGE
// #define FILTER_BATT_DATA

enum {
    kBorderImgGreen,
    kBorderImgYellow,
    kBorderImgRed,
    kNumBorderImages
};

enum {
    kFillTop,
    kFillMid,
    kFillBtm,
    kNumFill,
};

// All values are expected in Volts
typedef struct Threshold {
    float Level_High;
    float Level_Mid;
    float Level_Low;
    float Level_Depleted;
} Thresholds_t;

// There's a ~400mV drop loaded down by the ADC mux (SN74LVC1G3157DSFR).
// Originally tuned for 3x 1.5V AA, but later tuned for 3x 1.2V AA.
const float kAA_Level_High_V = 3.60f;
const float kAA_Level_Mid_V  = 3.15f;
const float kAA_Level_Low_V  = 2.80f;
const float kAA_Level_Depleted_V = 2.60f;

const float kLiPo_Level_High_V = 3.75f;
const float kLiPo_Level_Mid_V  = 3.40f;
const float kLiPo_Level_Low_V  = 3.10f;
const float kLiPo_Level_Depleted_V = 3.00f;

// Device limitation
const float kLevel_MinValid_V = 2.0f;

const Thresholds_t _Thresholds[kNumBatteryKind] = {
    [kBatteryKind_AA] = {
        .Level_High     = kAA_Level_High_V,
        .Level_Mid      = kAA_Level_Mid_V,
        .Level_Low      = kAA_Level_Low_V,
        .Level_Depleted = kAA_Level_Depleted_V,
    },
    [kBatteryKind_LiPo] = {
        .Level_High     = kLiPo_Level_High_V,
        .Level_Mid      = kLiPo_Level_Mid_V,
        .Level_Low      = kLiPo_Level_Low_V,
        .Level_Depleted = kLiPo_Level_Depleted_V,
    },
};

enum {
    kWidget_XPos_px = 53u,
    kWidget_YPos_px = 117u,

    kWidget_W_px = 106u,
    kWidget_H_px = 136u,

    kFill_XStart_px = 57u,
    kFill_XEnd_px = 99u,

    kColor_Border = 0x000000,

    // Three shades of red
    kColor_LightRed = 0xFF7660,
    kColor_Red = 0xFF0000,
    kColor_DarkRed = 0xCC0000,

    // Three shades of yellow
    kColor_LightYellow = 0xFFFF33,
    kColor_Yellow = 0xFFCC00,
    kColor_DarkYellow = 0xFF9900,

    // Three shades of green
    kColor_LightGreen = 0xC6FF00,
    kColor_Green = 0x80FF00,
    kColor_DarkGreen = 0x66CB01,

    kBattVSettleTime_ms = 7000,
};

typedef struct Battery {
    float battLevel_V;
    float battRawLevel_V;
    BatteryKind_t eBattKind;
    LipoChargeStatus_t LiPoIsCharging;
    bool firstDraw;
    lv_obj_t * battBorderImage;
    TickType_t BootTime_ticks;

    #ifdef FILTER_BATT_DATA
    float FilterAlpha;
    #endif

    #ifdef BATT_DISP_TEST
    uint8_t TestMode;
    #endif

    #ifdef BATT_DISP_VOLTAGE
    lv_obj_t* pBattVoltageText;
    lv_obj_t* pBattRawVoltageText;
    #endif

    lv_obj_t* pIconChargingObj;

} Battery_t;

static Battery_t _ctx;

const char* TAG = "battery";

static Line_t _fill[kNumFill] = {
    [kFillTop] = { .points = { {kFill_XStart_px, 120}, {kFill_XEnd_px, 120} }, .width = 2, /* color is set by level*/ },
    [kFillMid] = { .points = { {kFill_XStart_px, 122}, {kFill_XEnd_px, 122} }, .width = 5, /* color is set by level*/ },
    [kFillBtm] = { .points = { {kFill_XStart_px, 127}, {kFill_XEnd_px, 127} }, .width = 6, /* color is set by level*/ },
};

LV_IMG_DECLARE(icon_bat_large_g);
LV_IMG_DECLARE(icon_bat_large_y);
LV_IMG_DECLARE(icon_bat_large_r);
LV_IMG_DECLARE(icon_charging);

const void * _batt_borders[kNumBorderImages] = {
    &icon_bat_large_g,
    &icon_bat_large_y,
    &icon_bat_large_r
};

static OSD_Result_t Battery_Draw(void* arg);
static void set_fill_color(const float batt_V, const BatteryKind_t eKind);
static void set_fill_level(const uint32_t i, Line_t *const pFill, const float batt_V, const BatteryKind_t eKind);
static bool hasPowerUpSettlingElapsed(void);

#ifdef FILTER_BATT_DATA
static float calculate_alpha(const float Cutoff_Freq_Hz, const float SampleRate_Hz);
static float low_pass_filter(float NewInput, float *pPrevOutput, float Alpha);
#endif

OSD_Result_t Battery_Init(OSD_Widget_t *const pWidget)
{
    if (pWidget == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

#if defined(ESP_PLATFORM)
    _ctx.BootTime_ticks = xTaskGetTickCount();
#endif

    _ctx.firstDraw = true;

    // Assume no charge and AA chemistry until we get enough data from the FPGA
    _ctx.eBattKind   = kBatteryKind_AA;
    _ctx.battLevel_V = kLevel_MinValid_V;

    #ifdef BATT_DISP_TEST
    _ctx.TestMode = 1;
    #endif

    #ifdef FILTER_BATT_DATA
    _ctx.FilterAlpha = calculate_alpha(CUTOFF_FREQ_HZ, SAMPLING_INTERVAL_HZ);
    #endif

    pWidget->fnDraw = Battery_Draw;

    return kOSD_Result_Ok;
}

void Battery_UpdateVoltage(const float batt_V, const BatteryKind_t eKind)
{
    _ctx.battRawLevel_V = batt_V;

    if ((unsigned) eKind >= kNumBatteryKind)
    {
        return;
    }

    if (batt_V < kLevel_MinValid_V)
    {
        // prevent wrong initial measurement to be shown
        return;
    }

    #ifdef BATT_DISP_TEST
    if (_ctx.TestMode != 0)
    {
        if (_ctx.TestMode == 1)
        {
            _ctx.battLevel_V += 0.1;
            if (_ctx.battLevel_V > _Thresholds[_ctx.eBattKind].Level_High)
            {
                _ctx.TestMode = -1;
            }
        }
        else
        {
            _ctx.battLevel_V -= 0.1;
            if (_ctx.battLevel_V < _Thresholds[_ctx.eBattKind].Level_Depleted)
            {
                _ctx.TestMode = 1;
            }
        }

        // Do not update _ctx.eBattKind in test mode
    }
    else
    #endif
    {
        if (Mutex_Take(kMutexKey_Battery) == kMutexResult_Ok)
        {
            #ifdef FILTER_BATT_DATA
            if (eKind == kBatteryKind_AA)
            {
                static float PrevOutput_AA_V = kLevel_MinValid_V;
                const float FilteredLevel_V = low_pass_filter(batt_V, &PrevOutput_AA_V, _ctx.FilterAlpha);
                _ctx.battLevel_V = FilteredLevel_V;
            }
            else if (eKind == kBatteryKind_LiPo)
            {
                static float PrevOutput_LiPo_V = kLevel_MinValid_V;
                const float FilteredLevel_V = low_pass_filter(batt_V, &PrevOutput_LiPo_V, _ctx.FilterAlpha);
                _ctx.battLevel_V = FilteredLevel_V;
            }
            #else
            _ctx.battLevel_V = batt_V;
            #endif

            _ctx.eBattKind = eKind;

            (void) Mutex_Give(kMutexKey_Battery);
        }
    }
}

void Battery_SetChargingStatus(const bool IsCharging)
{
    #ifdef BATT_DISP_TEST
    static uint32_t ticks = 0;
    ticks++;
    if (ticks > 100)
    {
        ticks = 0;
        _ctx.LiPoIsCharging ^= 1;
    }
    #else
    if (Mutex_Take(kMutexKey_Battery) == kMutexResult_Ok)
    {
        _ctx.LiPoIsCharging = (IsCharging ? kLipoChargeStatus_Enabled : kLipoChargeStatus_Disabled);
        (void) Mutex_Give(kMutexKey_Battery);
    }
    #endif
}

static bool hasPowerUpSettlingElapsed(void)
{
    const TickType_t TimeElapsed_ticks = xTaskGetTickCount() - _ctx.BootTime_ticks;
    return (pdTICKS_TO_MS(TimeElapsed_ticks) > kBattVSettleTime_ms);
}

static OSD_Result_t Battery_Draw(void* arg)
{
    if (arg == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    lv_obj_t *pScreen = (lv_obj_t*)arg;

    #ifdef BATT_DISP_VOLTAGE
    if (_ctx.pBattVoltageText == NULL)
    {
        _ctx.pBattVoltageText = lv_label_create(pScreen);

        lv_obj_add_style(_ctx.pBattVoltageText, OSD_GetStyleTextWhite(), 0);
        lv_obj_align(_ctx.pBattVoltageText, LV_ALIGN_TOP_LEFT,
            kWidget_XPos_px - (kWidget_W_px - kWidget_XPos_px) / 2,
            kWidget_YPos_px + (kWidget_H_px - kWidget_YPos_px) / 2
            );
    }
    else
    {
        lv_label_set_text_fmt(_ctx.pBattVoltageText, "F = %1.3fV\nC = %d", BatteryLevel_V, _ctx.LiPoIsCharging);
        lv_obj_move_foreground(_ctx.pBattVoltageText);
    }

    if (_ctx.pBattRawVoltageText == NULL)
    {
        _ctx.pBattRawVoltageText = lv_label_create(pScreen);

        lv_obj_add_style(_ctx.pBattRawVoltageText, OSD_GetStyleTextWhite(), 0);
        lv_obj_align(_ctx.pBattRawVoltageText, LV_ALIGN_TOP_LEFT,
            kWidget_XPos_px + 2 * (kWidget_W_px - kWidget_XPos_px) / 2,
            kWidget_YPos_px + (kWidget_H_px - kWidget_YPos_px) / 2
            );
    }
    else
    {
        lv_label_set_text_fmt(_ctx.pBattRawVoltageText, "R = %1.3fV\nK = %d", _ctx.battRawLevel_V, _ctx.eBattKind);
        lv_obj_move_foreground(_ctx.pBattRawVoltageText);
    }
    #endif

    float BatteryLevel_V = 0.0f;

    if (Mutex_Take(kMutexKey_Battery) == kMutexResult_Ok)
    {
        BatteryLevel_V = _ctx.battLevel_V;
        (void) Mutex_Give(kMutexKey_Battery);
    }

#if defined(ESP_PLATFORM)
    // Wait a bit before displaying the battery widget to ensure the FPGA battery voltage sensing stabilizes.
    if ( (BatteryLevel_V <= kLevel_MinValid_V) && (hasPowerUpSettlingElapsed() == false))
    {
        return kOSD_Result_Ok;
    }
#endif

    if (_ctx.firstDraw)
    {
        _ctx.firstDraw = false;
        _ctx.battBorderImage = lv_img_create(pScreen);
        lv_obj_align(_ctx.battBorderImage, LV_ALIGN_CENTER, -1, 52);
    }

    // Draw battery meter
    set_fill_color(BatteryLevel_V, _ctx.eBattKind);

    for (size_t i=0; i<ARRAY_SIZE(_fill); i++)
    {
        if (_fill[i].pObj == NULL)
        {
            // Cache the line object the very first time. The line needs to persist for the duration of the draw event
            _fill[i].pObj = lv_line_create(pScreen);
            lv_obj_set_style_line_width(_fill[i].pObj, _fill[i].width, LV_PART_MAIN);
        }

        set_fill_level(i, &_fill[i], BatteryLevel_V, _ctx.eBattKind);

        lv_line_set_points(_fill[i].pObj, _fill[i].points, kLineConst_NumPoints);
        lv_obj_set_style_line_color(_fill[i].pObj, lv_color_hex(_fill[i].color), LV_PART_MAIN);
    }

    if (_ctx.pIconChargingObj == NULL)
    {
        _ctx.pIconChargingObj = lv_img_create(pScreen);
    }

    if (_ctx.LiPoIsCharging == kLipoChargeStatus_Enabled)
    {
        lv_img_set_src(_ctx.pIconChargingObj, &icon_charging);
        lv_obj_align(_ctx.pIconChargingObj, LV_ALIGN_CENTER, -1, 52);
    }
    else
    {
        if (_ctx.pIconChargingObj != NULL)
        {
            lv_obj_del(_ctx.pIconChargingObj);
            _ctx.pIconChargingObj = NULL;
        }
    }

    return kOSD_Result_Ok;
}

static void set_fill_color(const float batt_V, const BatteryKind_t eKind)
{
    if (batt_V > _Thresholds[eKind].Level_Mid)
    {
        _fill[kFillTop].color = kColor_LightGreen;
        _fill[kFillMid].color = kColor_Green;
        _fill[kFillBtm].color = kColor_DarkGreen;
        lv_img_set_src(_ctx.battBorderImage, _batt_borders[kBorderImgGreen]);
    }
    else if (batt_V > _Thresholds[eKind].Level_Low)
    {
        _fill[kFillTop].color = kColor_LightYellow;
        _fill[kFillMid].color = kColor_Yellow;
        _fill[kFillBtm].color = kColor_DarkYellow;
        lv_img_set_src(_ctx.battBorderImage, _batt_borders[kBorderImgYellow]);
    }
    else
    {
        _fill[kFillTop].color = kColor_LightRed;
        _fill[kFillMid].color = kColor_Red;
        _fill[kFillBtm].color = kColor_DarkRed;
        lv_img_set_src(_ctx.battBorderImage, _batt_borders[kBorderImgRed]);
    }
}

static void set_fill_level(const uint32_t i, Line_t *const pFill, const float batt_V, const BatteryKind_t eKind)
{
    if (pFill == NULL)
    {
        ESP_LOGW(TAG, "battery fill null data ptr");
        return;
    }

    static float prevFillBattLevels[kNumFill] = {0.0f};

    // Only update the fill level when the battery charge changes
    if (prevFillBattLevels[i] != batt_V)
    {
        prevFillBattLevels[i] = batt_V;
    }
    else
    {
        return;
    }

    lv_point_t const *const pStartPoint = &pFill->points[kLineConst_PointStartIdx];
    lv_point_t *const pEndPoint = &pFill->points[kLineConst_PointEndIdx];

    const float Depleted_V = _Thresholds[eKind].Level_Depleted;
    const float High_V = _Thresholds[eKind].Level_High;

    float percent_filled = (batt_V - Depleted_V) / (High_V - Depleted_V);

    if (percent_filled >= 0.95f)
    {
        percent_filled = 1.0f;
    }
    else if (percent_filled < 0.03f)
    {
        percent_filled = 0.03f;
    }

    pEndPoint->x = pStartPoint->x + (uint32_t)(percent_filled * (kFill_XEnd_px - kFill_XStart_px));

    if (pEndPoint->x < pStartPoint->x)
    {
        ESP_LOGW(TAG, "fill end pos cannot be less than start pos");
    }

}

#ifdef FILTER_BATT_DATA
// Function to calculate the smoothing factor alpha
static float calculate_alpha(const float Cutoff_Freq_Hz, const float SampleRate_Hz)
{
    const float PI = 3.14159265358979f;
    float RC = 1.0f / (2.0f * PI * Cutoff_Freq_Hz);
    float dt = 1.0f / SampleRate_Hz;
    float alpha = dt / (RC + dt);

    ESP_LOGD(TAG, "Sample Rate: %f Hz, Cutoff Frequency: %f Hz, Alpha: %f", SampleRate_Hz, Cutoff_Freq_Hz, alpha);

    return alpha;
}

// Function to apply the low-pass filter
static float low_pass_filter(float NewInput, float *pPrevOutput, float Alpha)
{
    float Output = Alpha * NewInput + (1.0f - Alpha) * (*pPrevOutput);
    *pPrevOutput = Output;  // Update the previous Output for the next sample
    return Output;
}
#endif
