#include "tab_list.h"
#include "tab_shared.h"

#include "color.h"
#include "dlist.h"
#include "osd_shared.h"

#include "lvgl.h"
#include "esp_log.h"

#include <stdint.h>
#include <string.h>

LV_IMG_DECLARE(img_arrow_up);
LV_IMG_DECLARE(img_arrow_down);

// LVGL v8 has a bug where it cannot draw 1px images. These are instead 2px squares with pixel (0,0) set.
LV_IMG_DECLARE(img_dot_grey);
LV_IMG_DECLARE(img_dot_white);

enum {
    kCenterDot_px = 70,
    kDotGap_px    = 5,
    kDotPosX_px   = 18,
};

static lv_obj_t* pArrowUpObj;
static lv_obj_t* pArrowDownObj;

static const char* TAG = "TabDot";

OSD_Result_t TabDot_Draw(void* arg)
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
        ESP_LOGE(TAG, "TabDot current item is null");
        return kOSD_Result_Err_NullDataPtr;
    }

    const size_t NumItems = sys_dlist_len(&pList->WidgetList);

    // There is no "middle dot" when an even list of items are present. The middle is slightly in between two centered dots.
    const uint8_t MiddleDotPos = ((NumItems % 2) != 0) ? kCenterDot_px : ( kCenterDot_px - (kDotGap_px/2) );
    const uint8_t SpaceForTopHalfDots = (NumItems <= 1) ? 0 : ( (NumItems/2 - 1) * kDotGap_px );

    lv_point_t FirstDot = {
        .x = kDotPosX_px,
        .y = MiddleDotPos - SpaceForTopHalfDots,
    };

    if (NumItems > ((kCenterDot_px / kDotGap_px ) - 1))
    {
        ESP_LOGE(TAG, "Number of items in tab dot exceeded");
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
                pItem->DataObj = lv_img_create(pScreen);
            }

            const lv_point_t DotPos = {
                .x = FirstDot.x,
                .y = FirstDot.y + ( i * kDotGap_px ),
            };

            lv_obj_align(pItem->DataObj, LV_ALIGN_TOP_LEFT, DotPos.x, DotPos.y);

            if (pList->pCurrent == pItem)
            {
                lv_img_set_src(pItem->DataObj, &img_dot_white);
            }
            else
            {
                lv_img_set_src(pItem->DataObj, &img_dot_grey);
            }

            i++;
        }
    }

    const lv_coord_t Arrows_X_pos = FirstDot.x - img_arrow_up.header.w/2;
    const lv_point_t ArrowUpPos = {Arrows_X_pos, FirstDot.y - kDotGap_px - img_arrow_up.header.h};
    const lv_point_t ArrowDownPos = {Arrows_X_pos, FirstDot.y + (NumItems * kDotGap_px) + 1};

    if (pArrowUpObj == NULL)
    {
        pArrowUpObj = lv_img_create(pScreen);
        lv_img_set_src(pArrowUpObj, &img_arrow_up);
        lv_obj_align(pArrowUpObj, LV_ALIGN_TOP_LEFT, ArrowUpPos.x, ArrowUpPos.y);
    }

    if (pArrowDownObj == NULL)
    {
        pArrowDownObj = lv_img_create(pScreen);
        lv_img_set_src(pArrowDownObj, &img_arrow_down);
        lv_obj_align(pArrowDownObj, LV_ALIGN_TOP_LEFT, ArrowDownPos.x, ArrowDownPos.y);
    }

    // Draw the right-hand-side content associated with this menu item
    if (pList->pCurrent->Widget.fnDraw != NULL)
    {
        pList->pCurrent->Widget.fnDraw(pScreen);
    }

    return kOSD_Result_Ok;
}

OSD_Result_t TabDot_OnTransition(void *arg)
{
    if (arg == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    TabCollection_t* pList = (TabCollection_t*)(arg);

    lv_obj_t** ToDelete[] = {
        &pList->pDividerObj,
        &pList->pSelectedObj,
        &pArrowUpObj,
        &pArrowDownObj,
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
