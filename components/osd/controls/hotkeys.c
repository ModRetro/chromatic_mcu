#include "hotkeys.h"

#include "esp_log.h"
#include "osd_shared.h"
#include "lvgl.h"

typedef enum HKID {
    kHKID_Brightness,
    kHKID_ResetEmuCore,
    kNumHotKeys
} HKID_t;

typedef struct HKEntry {
    lv_obj_t* pNameTextObj;
    lv_obj_t* pDescrTextObj;
    const char* pName;
    const char* pDescr;
} HKEntry_t;

static lv_obj_t* _pMenuTitleTextObj;
static HKEntry_t _HotKeys[kNumHotKeys] = {
    [kHKID_Brightness] = {
        .pName = "BRIGHTNESS",
        .pDescr = "MENU + L/R ON DPAD",
    },
    [kHKID_ResetEmuCore] = {
        .pName = "RESET EMULATION",
        .pDescr = "MENU + B + A + START + SELECT",
    }
};

__attribute__((unused)) static const char* TAG = "HK";

OSD_Result_t HotKeys_Draw(void* arg)
{
    if (arg == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    lv_obj_t *const pScreen = (lv_obj_t *const) arg;

    enum {
        kText_X_px = 77,
        kText_Y_px = 45,
        kText_Y_Adjust_px  = 25,
        kMaxTextWidth_px = 63,
    };

    if (_pMenuTitleTextObj == NULL)
    {
        _pMenuTitleTextObj = lv_label_create(pScreen);
        lv_obj_align(_pMenuTitleTextObj, LV_ALIGN_TOP_LEFT, 36, 52);
        lv_label_set_long_mode(_pMenuTitleTextObj, LV_LABEL_LONG_WRAP);
        lv_obj_set_style_text_align(_pMenuTitleTextObj, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(_pMenuTitleTextObj, 20);
        lv_obj_add_style(_pMenuTitleTextObj, OSD_GetStyleTextBlack(), 0);
        lv_label_set_text_static(_pMenuTitleTextObj, "HOT KEYS");
    }

    for (size_t i=0; i < ARRAY_SIZE(_HotKeys); i++)
    {
        HKEntry_t *const pHK = &_HotKeys[i];

        if (pHK->pNameTextObj == NULL)
        {
            pHK->pNameTextObj = lv_label_create(pScreen);

            lv_obj_align(pHK->pNameTextObj, LV_ALIGN_TOP_LEFT, kText_X_px, kText_Y_px + (kText_Y_Adjust_px * i));
            lv_label_set_long_mode(pHK->pNameTextObj, LV_LABEL_LONG_WRAP);
            lv_obj_set_style_text_align(pHK->pNameTextObj, LV_TEXT_ALIGN_LEFT, 0);
            lv_obj_set_width(pHK->pNameTextObj, kMaxTextWidth_px);

            lv_obj_add_style(pHK->pNameTextObj, OSD_GetStyleTextWhite(), 0);
            lv_label_set_text_static(pHK->pNameTextObj, pHK->pName);
        }

        if (pHK->pDescrTextObj == NULL)
        {
            pHK->pDescrTextObj = lv_label_create(pScreen);

            lv_obj_align(pHK->pDescrTextObj, LV_ALIGN_TOP_LEFT, kText_X_px, kText_Y_px  + 8 + (kText_Y_Adjust_px * i) );
            lv_label_set_long_mode(pHK->pDescrTextObj, LV_LABEL_LONG_WRAP);
            lv_obj_set_style_text_align(pHK->pDescrTextObj, LV_TEXT_ALIGN_LEFT, 0);
            lv_obj_set_width(pHK->pDescrTextObj, kMaxTextWidth_px);

            lv_obj_add_style(pHK->pDescrTextObj, OSD_GetStyleTextGrey(), 0);
            lv_label_set_text_static(pHK->pDescrTextObj, pHK->pDescr);
        }
    }

    return kOSD_Result_Ok;
}

OSD_Result_t HotKeys_OnButton(const Button_t Button, const ButtonState_t State, void *arg)
{
    (void)arg;
    (void)Button;
    (void)State;

    // TODO: Use A/B to cycle through pages as more hot keys are added

    return kOSD_Result_Ok;
}

OSD_Result_t HotKeys_OnTransition(void* arg)
{
    (void)arg;

    for (size_t i = 0; i < ARRAY_SIZE(_HotKeys); i++)
    {
        if (_HotKeys[i].pNameTextObj != NULL)
        {
            lv_obj_del(_HotKeys[i].pNameTextObj);
            _HotKeys[i].pNameTextObj = NULL;
        }

        if (_HotKeys[i].pDescrTextObj != NULL)
        {
            lv_obj_del(_HotKeys[i].pDescrTextObj);
            _HotKeys[i].pDescrTextObj = NULL;
        }
    }

    if (_pMenuTitleTextObj != NULL)
    {
        lv_obj_del(_pMenuTitleTextObj);
        _pMenuTitleTextObj = NULL;
    }

    return kOSD_Result_Ok;
}
