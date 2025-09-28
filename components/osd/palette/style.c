#include "style.h"

#include "esp_log.h"
#include "osd_shared.h"
#include "tab_shared.h"

LV_IMG_DECLARE(img_chromatic_eyebrow);

enum {
    kNumRows     = 8,
    kNumCols     = 2,
    kTblCol0W_px = 56,
    kTblCol1W_px = 71,
    kRowPadL_px  = 16,

    // GBC incompatible positioning
    kGBCTxtOffsetX_px = 77,
    kGBCTxtOffsetY_px = 45,
    kGBCTxtW_px       = 60,
    kGBCImgOffsetX_px = 29,
    kGBCImgOffsetY_px = 43,

    // Color preview
    kClrPrvOffsetX_px = 7,
    kClrPrvOffsetY_px = 0,
    kClrPrvW_px       = 5,
    kClrPrvH_px       = 5,

    // Current palette arrow indicator
    kNdctrNumPts     = 3,
    kNdctrW_px       = 3,
    kNdctrH_px       = 6,
    kNdctrBorderW_px = 2,  // Needs to be 2 for LVGL to render triangle properly
};

// Palette style preview colors
static const uint32_t PaletteStyleColors[kNumPalettes] = {
    [kPalette_Default]   = 0x999999,
    [kPalette_Brown]     = 0xE8A974,
    [kPalette_Blue]      = 0x52A1FF,
    [kPalette_Pastel]    = 0xD6CAE9,
    [kPalette_Green]     = 0x72933B,
    [kPalette_Red]       = 0xBC524E,
    [kPalette_DarkBlue]  = 0x134284,
    [kPalette_Orange]    = 0xEF8228,
    [kPalette_DarkGreen] = 0x29540D,
    [kPalette_DarkBrown] = 0x723809,
    [kPalette_Grayscale] = 0x939393,
    [kPalette_Yellow]    = 0xFBE894,
    [kPalette_Negative]  = 0xFFFFFF, 
    [kPalette_DMG1]      = 0xB8C1A0,
    [kPalette_DMG2]      = 0x998E30
};

//  Taken from chromatic_hdl: esp32t/src/rtl/EMU/CORE/BootROMs/cgb_boot.asm#L501
//  color order is reversed in the palette style
//  24-bit colors can be converted to 15-bit from https://tcrf.net/Notes:Game_Boy_Color_Bootstrap_ROM
static const uint64_t PaletteStyleBG[kNumPalettes] = {
    [kPalette_Default]   = 0x8000000000000000,  // Default color is obtained by turning off the custom palette bit
    [kPalette_Brown]     = 0x000000D032BF7FFF,  // Palette: 0
    [kPalette_Blue]      = 0x00007C007E8C7FFF,  // Palette: 28
    [kPalette_Pastel]    = 0x00007E524A5F53FF,  // Palette: 12
    [kPalette_Green]     = 0x0000011F03EA7FFF,  // Palette: 18
    [kPalette_Red]       = 0x00001CF2421F7FFF,  // Palette: 4
    [kPalette_DarkBlue]  = 0x0000454A6E317FFF,  // Palette: 2
    [kPalette_Orange]    = 0x0000001F03FF7FFF,  // Palette: 24
    [kPalette_DarkGreen] = 0x000061801BEF7FFF,  // Palette: 29
    [kPalette_DarkBrown] = 0x04CB15B04279639F,  // Palette: 1
    [kPalette_Grayscale] = 0x0000294A52947FFF,  // Palette: 5
    [kPalette_Yellow]    = 0x0000012F03FF7FFF,  // Palette: 6
    [kPalette_Negative]  = 0x7FFF037442000000,  // Palette: 27
    [kPalette_DMG1]      = 0x1DC02A603B4043E0,  // Palette: 30
    [kPalette_DMG2]      = 0x00E20147018B01EF   // Palette: 31
};

typedef struct PaletteCtl {
    StyleID_t CurrStyleID;
    StyleID_t SavedStyleID;
    StyleID_t HotKeyStyleID;
    fnOnUpdateCb_t fnOnUpdateCb;
    bool GBCMode;   // True if the current game is a GBC game
    bool Initialized;
    bool TableCbAdded;
} PaletteCtl_t;

static const char* TAG = "PaletteCtl";
static PaletteCtl_t _Ctx;

static void SaveToSettings(const StyleID_t ID );
static void StyleOnEventDrawCb(lv_event_t * e);

OSD_Result_t Style_Draw(void* arg)
{
    if (arg == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    lv_obj_t* pTable = (lv_obj_t*)((TabTable_DrawCtx_t*)arg)->pTable;
    TabItem_t* pItem = (TabItem_t*)((TabTable_DrawCtx_t*)arg)->pItem;
    const StyleID_t CurrID = (uint16_t)((TabTable_DrawCtx_t*)arg)->CurrID;

    if ( (pTable == NULL) || (pItem == NULL) || (pItem->DataObj == NULL) )
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    // When in GBC mode, render eyebrow and text msg
    if (_Ctx.GBCMode)
    {
        // Style_Draw is called kNumPalette times. When in GBC mode, we only want to draw the eyebrow and text once
        if (CurrID == kPalette_Default)
        {
            lv_obj_t* pEyebrow = lv_img_create(pItem->DataObj); 
            lv_img_set_src(pEyebrow, &img_chromatic_eyebrow);
            lv_obj_align(pEyebrow, LV_ALIGN_TOP_LEFT, kGBCImgOffsetX_px, kGBCImgOffsetY_px);

            lv_obj_t* pText = lv_label_create(pItem->DataObj);
            lv_label_set_text_static(pText, "YOUR GAME DOES\nNOT SUPPORT\nCOLOR PALETTES");
            lv_obj_align(pText, LV_ALIGN_TOP_LEFT, kGBCTxtOffsetX_px, kGBCTxtOffsetY_px);
            lv_obj_add_style(pText, OSD_GetStyleTextWhite(), LV_PART_MAIN);
            lv_obj_set_width(pText, kGBCTxtW_px);
        } 
    } else {
        const char* CellText;

        if ((_Ctx.HotKeyStyleID != kPalette_Default) &&  // Hotkey is used
            (_Ctx.SavedStyleID == kPalette_Default) &&   // No saved style
            (CurrID % kNumRows == 0) &&                  // Row 0 
            (CurrID / kNumRows == 0))                    // Col 0
        {
            CellText = "HOTKEY";
        }
        else
        {
            CellText = pItem->Widget.Name;
        }

        lv_table_set_cell_value(pTable, CurrID % kNumRows, CurrID / kNumRows, CellText);

        // Draw callback only needs to be added to table once
        if (!_Ctx.TableCbAdded)
        {
            lv_table_set_col_width(pTable, 0, kTblCol0W_px);
            lv_table_set_col_width(pTable, 1, kTblCol1W_px);
            lv_table_set_row_cnt(pTable, kNumRows);  //Not required but avoids a lot of memory reallocation lv_table_set_set_value
            lv_table_set_col_cnt(pTable, kNumCols);

            lv_obj_set_style_pad_all(pTable, 0, LV_PART_ITEMS);
            lv_obj_set_style_pad_gap(pTable, 0, LV_PART_ITEMS);
            lv_obj_set_style_pad_left(pTable, kRowPadL_px, LV_PART_ITEMS);
            lv_obj_set_style_pad_bottom(pTable, 0, LV_PART_ITEMS);

            lv_obj_set_style_border_width(pTable, 0, LV_PART_MAIN);
            lv_obj_set_style_border_width(pTable, 0, LV_PART_ITEMS);
            lv_obj_set_style_bg_opa(pTable, LV_OPA_0, LV_PART_MAIN);
            lv_obj_set_style_bg_opa(pTable, LV_OPA_0, LV_PART_ITEMS);
            lv_obj_add_style(pTable, OSD_GetStyleTextWhite(), LV_PART_ITEMS);

            // Palette color, saved indicator, and current palette indicator are
            // directly drawn in the table cell WITHOUT creating objects for them
            lv_obj_add_event_cb(pTable, StyleOnEventDrawCb, LV_EVENT_DRAW_PART_BEGIN, NULL);

            _Ctx.TableCbAdded = true;
        }
    }

    return kOSD_Result_Ok;
}

void Style_Update(const StyleID_t NewID)
{
    if ((unsigned)NewID >= kNumPalettes)
    {
        ESP_LOGE(TAG, "Invalid style ID: %u", NewID);
        return;
    }

    _Ctx.CurrStyleID = NewID;

    // Notify the registered callback about the style update
    if (_Ctx.fnOnUpdateCb != NULL)
    {
        _Ctx.fnOnUpdateCb();
    }
    else
    {
        ESP_LOGW(TAG, "No callback registered for style updates");
    }
}

OSD_Result_t Style_OnButton(const Button_t Button, const ButtonState_t State, void *arg)
{ 
    (void)arg;

    if (!_Ctx.GBCMode)
    {
        switch (Button)
        {
            case kButton_A:
                if (State == kButtonState_Pressed)
                {
                    _Ctx.SavedStyleID = _Ctx.CurrStyleID;
                    SaveToSettings(_Ctx.SavedStyleID);
                }
                break;
            case kButton_Down:
                if (State == kButtonState_Pressed)
                {
                    const StyleID_t NewID = (_Ctx.CurrStyleID + 1) % kNumPalettes;
                    Style_Update(NewID);
                }
                break;
            case kButton_Up:
                if (State == kButtonState_Pressed)
                {
                    const StyleID_t NewID = _Ctx.CurrStyleID > 0 ? _Ctx.CurrStyleID - 1 : kNumPalettes - 1;
                    Style_Update(NewID);
                }
                break;
            default:
                break;
        }
    }

    return kOSD_Result_Ok;
}

OSD_Result_t Style_OnTransition(void* arg)
{
    (void)arg;

    // Callback is removed when table is destroyed, so we need to clear flag to re-add the callback
    _Ctx.TableCbAdded = false;

    return kOSD_Result_Ok;
}

uint64_t Style_GetPaletteBG(const StyleID_t ID)
{
    if ((unsigned)ID >= kNumPalettes)
    {
        ESP_LOGE(TAG, "Invalid palette index: %u", ID);
        return PaletteStyleBG[kPalette_Default];  // Return default background if index is invalid
    }

    return PaletteStyleBG[ID];
}

StyleID_t Style_GetCurrID(void)
{
    return _Ctx.CurrStyleID;
}

OSD_Result_t Style_ApplySetting(const SettingValue_t* pValue)
{
    if (pValue == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    if (pValue->eType == kSettingDataType_U8)
    {
        _Ctx.SavedStyleID = pValue->U8;
    }
    else
    {
        return kOSD_Result_Err_UnexpectedSettingDataType;
    }

    return kOSD_Result_Ok;
}

void Style_RegisterOnUpdateCb(fnOnUpdateCb_t fnOnUpdate)
{
    _Ctx.fnOnUpdateCb = fnOnUpdate;
}

void Style_SetGBCMode(const bool GBCMode)
{
    _Ctx.GBCMode = GBCMode;
}

void Style_SetHKPaletteBG(const uint64_t paletteBG)
{
    for (StyleID_t i = 0; i < kNumPalettes; i++) {
        if (PaletteStyleBG[i] == paletteBG)
        {
            _Ctx.HotKeyStyleID = i;
            break;
        }
    }
}

bool Style_IsInitialized(void)
{
    return _Ctx.Initialized;
}

void Style_Initialize(void)
{
    // Priority is hot key palette over saved palette
    // Enforce that hot key palette selection should only apply when saved palette is default
    if ((_Ctx.HotKeyStyleID != kPalette_Default) && (_Ctx.SavedStyleID == kPalette_Default))
    {
        Style_Update(_Ctx.HotKeyStyleID);
        ESP_LOGI(TAG, "Palette initialized to: Hotkey palette=%u", _Ctx.HotKeyStyleID);
    } 
    else if (_Ctx.SavedStyleID != kPalette_Default)
    {
        Style_Update(_Ctx.SavedStyleID);
        ESP_LOGI(TAG, "Palette initialized to: Saved palette=%u", _Ctx.SavedStyleID);
    }

    _Ctx.Initialized = true;
}

static void SaveToSettings(const StyleID_t ID)
{
    OSD_Result_t eResult;

    if ((eResult = Settings_Update(kSettingKey_PaletteStyleID, ID)) != kOSD_Result_Ok)
    {
        ESP_LOGE(TAG, "Palette style save failed: %d", eResult);
    }
}

static void StyleOnEventDrawCb(lv_event_t * e)
{
    lv_obj_t* pTable = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t* pDsc = lv_event_get_draw_part_dsc(e);

    if(pDsc->part == LV_PART_ITEMS) 
    {
        // pDsc->id is incremented in row-major order
        const uint32_t row = pDsc->id / lv_table_get_col_cnt(pTable);
        const uint32_t col = pDsc->id - row * lv_table_get_col_cnt(pTable);
        const StyleID_t ID = row + (col * kNumRows);

        if ((unsigned)ID < kNumPalettes)
        {
            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            rect_dsc.bg_color = lv_color_hex(PaletteStyleColors[ID]);

            // Color preview
            lv_area_t sw_area;
            sw_area.x1 = pDsc->draw_area->x1 + kClrPrvOffsetX_px;
            sw_area.y1 = pDsc->draw_area->y1 + kClrPrvOffsetY_px;
            sw_area.x2 = sw_area.x1 + kClrPrvW_px;
            sw_area.y2 = sw_area.y1 + kClrPrvH_px;
            lv_draw_rect(pDsc->draw_ctx, &rect_dsc, &sw_area);

            // Saved palette indicator
            if (ID == _Ctx.SavedStyleID)
            {
                rect_dsc.bg_opa = LV_OPA_0;
                rect_dsc.border_width = 1;
                rect_dsc.border_color = lv_color_black();

                sw_area.x1 = sw_area.x1 + 1;
                sw_area.y1 = sw_area.y1 + 1;
                sw_area.x2 = sw_area.x2 - 1;
                sw_area.y2 = sw_area.y2 - 1;
                lv_draw_rect(pDsc->draw_ctx, &rect_dsc, &sw_area);
            }

            // Current palette indicator
            if (ID == _Ctx.CurrStyleID)
            {
                lv_draw_rect_dsc_t triangle_dsc;
                lv_draw_rect_dsc_init(&triangle_dsc);
                triangle_dsc.border_color = lv_color_white();
                triangle_dsc.border_width = kNdctrBorderW_px;

                lv_point_t points[kNdctrNumPts] = {
                    {pDsc->draw_area->x1, pDsc->draw_area->y1},                           // Top-left corner
                    {pDsc->draw_area->x1, pDsc->draw_area->y1 + kNdctrH_px},              // Bottom-left corner
                    {pDsc->draw_area->x1 + kNdctrW_px, pDsc->draw_area->y1 + kNdctrW_px}  // Middle point
                };

                lv_draw_triangle(pDsc->draw_ctx, &triangle_dsc, points);
            }
        }
    }
}
