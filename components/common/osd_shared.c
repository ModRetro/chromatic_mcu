#include "osd_shared.h"
#include "color.h"
#include "lvgl.h"

LV_FONT_DECLARE(chibit_mr);
LV_FONT_DECLARE(fingfai);
LV_FONT_DECLARE(jf_dot_k14);

static lv_style_t _StyleTextWhite;
static lv_style_t _StyleTextGrey;
static lv_style_t _StyleTextBlack;
static lv_style_t _StyleTextWhite_L;

OSD_Result_t OSD_Common_Init(void)
{
    lv_style_init(&_StyleTextWhite);
    lv_style_init(&_StyleTextGrey);
    lv_style_init(&_StyleTextBlack);
    lv_style_init(&_StyleTextWhite_L);

    lv_style_set_text_color(&_StyleTextWhite, lv_color_hex(kColor_White));
    lv_style_set_text_font(&_StyleTextWhite, &fingfai);

    lv_style_set_text_color(&_StyleTextGrey,lv_color_hex(kColor_Grey));
    lv_style_set_text_font(&_StyleTextGrey, &fingfai);

    lv_style_set_text_color(&_StyleTextBlack,lv_color_hex(kColor_Black));
    lv_style_set_text_font(&_StyleTextBlack, &fingfai);

    lv_style_set_text_color(&_StyleTextWhite_L, lv_color_hex(kColor_White));
    lv_style_set_text_font(&_StyleTextWhite_L, &jf_dot_k14);

    return kOSD_Result_Ok;
}

lv_style_t* OSD_GetStyleTextWhite(void)
{
    return &_StyleTextWhite;
}

lv_style_t* OSD_GetStyleTextGrey(void)
{
    return &_StyleTextGrey;
}

lv_style_t* OSD_GetStyleTextBlack(void)
{
    return &_StyleTextBlack;
}

lv_style_t* OSD_GetStyleTextWhite_L(void)
{
    return &_StyleTextWhite_L;
}
