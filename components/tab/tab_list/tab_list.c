#include "tab_list.h"

#include "color.h"
#include "dlist.h"
#include "osd_shared.h"
#include "tab_shared.h"

#include "lvgl.h"
#include "esp_log.h"

#include <stdint.h>
#include <string.h>

enum {
    kColorDivider = 0x666666,
};

// All pixels relative to the top-level corner
enum {
    kMenuOriginX_px = 7,
    kMenuOriginY_px = 7,

    kListOriginX_px = 8,
    kListOriginY_px = 35,

    kTextOffsetX_px = 3,
    kTextOffsetY_px = 2,

    kTextAdjustX_px = 0,
    kTextAdjustY_px = 13,

    kDividerOffsetX_px      = 65,
    kDividerOffsetYStart_px = 35,
    kDividerOffsetYEnd_px   = 98,

    kDividerWidth_px = 1,

    kRectBoundWidth_px = 1,

    kRectSelectedXOffset_px = 3,
    kRectSelectedYOffset_px = 2,
    kRectSelectedH_px  = 11,
    kRectSelectedW_px  = 55,
};

static const char* TAG = "TabList";

static const lv_point_t MenuOrigin = {.x = kMenuOriginX_px, .y = kMenuOriginY_px};
static const lv_point_t ListOrigin = {.x = kListOriginX_px, .y = kListOriginY_px};
static const lv_point_t TextOffset = {.x = kTextOffsetX_px, .y = kTextOffsetY_px};
static const lv_point_t TextAdjust = {.x = kTextAdjustX_px, .y = kTextAdjustY_px};
static const lv_point_t DividerOffset[] = {
    {.x = kMenuOriginX_px + kDividerOffsetX_px, .y = kMenuOriginY_px + kDividerOffsetYStart_px},
    {.x = kMenuOriginX_px + kDividerOffsetX_px, .y = kMenuOriginY_px + kDividerOffsetYEnd_px},
};

OSD_Result_t TabList_Draw(void* arg)
{
    if (arg == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    TabCollection_t* pList = (TabCollection_t*)(((Tab_DrawCtx_t*)arg)->pTabList);
    lv_obj_t* pScreen = (lv_obj_t*)(((Tab_DrawCtx_t*)arg)->pScreen);

    if ( (pList == NULL) || (pScreen == NULL) )
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    if (pList->pCurrent == NULL)
    {
        ESP_LOGE(TAG, "TabList current item is null");
        return kOSD_Result_Err_NullDataPtr;
    }

    size_t i = 0;
    sys_dnode_t *pNode = NULL;
    SYS_DLIST_FOR_EACH_NODE(&pList->WidgetList, pNode) {
        if (pNode == NULL)
        {
            // List is empty or we are at the tail
            break;
        }
        else
        {
            OSD_Widget_t *const pWidget = (OSD_Widget_t*)pNode;
            TabItem_t *const pItem = (TabItem_t*)pWidget;

            if (pItem->DataObj == NULL)
            {
                pItem->DataObj = lv_label_create(pScreen);
            }

            if (pList->pDividerObj == NULL)
            {
                pList->pDividerObj = lv_line_create(pScreen);
                lv_obj_set_style_line_width(pList->pDividerObj, kDividerWidth_px, LV_PART_MAIN);
                lv_obj_set_style_line_color(pList->pDividerObj, lv_color_hex(kColorDivider), LV_PART_MAIN);
            }

            const lv_point_t TextOrigin = {
                .x = MenuOrigin.x + ListOrigin.x + TextOffset.x + ( i * TextAdjust.x ),
                .y = MenuOrigin.y + ListOrigin.y + TextOffset.y + ( i * TextAdjust.y ),
            };

            // Remove all styles so that the selected item is set appropriately
            lv_obj_remove_style_all(pItem->DataObj);
            lv_obj_align(pItem->DataObj, LV_ALIGN_TOP_LEFT, TextOrigin.x, TextOrigin.y);
            lv_label_set_text_static(pItem->DataObj, pItem->Widget.Name);
            lv_obj_move_foreground(pItem->DataObj);

            if (pList->pCurrent == pItem)
            {
                lv_obj_add_style(pItem->DataObj, OSD_GetStyleTextWhite(), 0);

                if (pList->pSelectedObj == NULL)
                {
                    pList->pSelectedObj = lv_obj_create(pScreen);

                    // Disable the default scroll bar
                    lv_obj_remove_style_all(pList->pSelectedObj);

                    const lv_color_t AccentColor = (((Tab_DrawCtx_t*)arg)->AccentColor);

                    lv_obj_set_size(pList->pSelectedObj, kRectSelectedW_px, kRectSelectedH_px);
                    lv_obj_set_pos(pList->pSelectedObj, TextOrigin.x - kRectSelectedXOffset_px, TextOrigin.y - kRectSelectedYOffset_px);
                    lv_obj_set_style_border_color(pList->pSelectedObj, AccentColor, 0);
                    lv_obj_set_style_border_width(pList->pSelectedObj, kRectBoundWidth_px, 0);
                    lv_obj_set_style_bg_opa(pList->pSelectedObj, LV_OPA_0, 0);
                }
            }
            else
            {
                lv_obj_add_style(pItem->DataObj, OSD_GetStyleTextGrey(), 0);
            }

            i++;
        }
    }

    if (pList->pDividerObj != NULL)
    {
        lv_line_set_points(pList->pDividerObj, DividerOffset, ARRAY_SIZE(DividerOffset));
        lv_obj_move_foreground(pList->pDividerObj);
    }

    // Draw the right-hand-side content associated with this menu item
    if (pList->pCurrent->Widget.fnDraw != NULL)
    {
        pList->pCurrent->Widget.fnDraw(pScreen);
    }

    return kOSD_Result_Ok;
}

OSD_Result_t TabList_OnTransition(void *arg)
{
    if (arg == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    TabCollection_t* pList = (TabCollection_t*)(arg);

    lv_obj_t** ToDelete[] = {
        &pList->pDividerObj,
        &pList->pCurrent->DataObj,
        &pList->pSelectedObj,
    };

    for (size_t i = 0; i < ARRAY_SIZE(ToDelete); i++)
    {
        if (*ToDelete[i] != NULL)
        {
            lv_obj_del(*ToDelete[i]);
            *(ToDelete[i]) = NULL;
        }
    }

    sys_dnode_t *pNode = NULL;
    SYS_DLIST_FOR_EACH_NODE(&pList->WidgetList, pNode) {
        if (pNode == NULL)
        {
            // List is empty or we are at the tail
            break;
        }
        else
        {
            OSD_Widget_t *const pWidget = (OSD_Widget_t*)pNode;
            TabItem_t *const pItem = (TabItem_t*)pWidget;

            if (pItem->DataObj != NULL)
            {
                lv_obj_del(pItem->DataObj);
                pItem->DataObj = NULL;
            }
        }
    }

    if (pList->pCurrent->Widget.fnOnTransition != NULL)
    {
        pList->pCurrent->Widget.fnOnTransition(NULL);
    }

    return kOSD_Result_Ok;
}
