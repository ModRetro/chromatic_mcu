#include "tab_table.h"

#include "tab_shared.h"
#include "esp_log.h"

enum {
    kTblOriginX_px = 19,
    kTblOriginY_px = 43,
};

static const char* TAG = "TabTable";

OSD_Result_t TabTable_Draw(void* arg)
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

    // Instantiating table into pSelectedObj since it updates on every TabNext/Prev
    if (pList->pSelectedObj == NULL)
    {
        lv_obj_t* pTable = lv_table_create(pScreen);
        lv_obj_remove_style_all(pTable);
        lv_obj_align(pTable, LV_ALIGN_TOP_LEFT, kTblOriginX_px, kTblOriginY_px);

        uint16_t i = 0;
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
                    pItem->DataObj = lv_obj_create(pScreen);
                    lv_obj_remove_style_all(pItem->DataObj);
                }

                // Handle individual item drawing and object creation internally
                TabTable_DrawCtx_t Tbl_Ctx = {
                    .pTable = pTable,  
                    .pItem = pItem,  // New objects must be children of pItem->DataObj
                    .CurrID = i,
                };
                pWidget->fnDraw(&Tbl_Ctx);

                i++;
            }
        }
        pList->pSelectedObj = pTable;
    }

    return kOSD_Result_Ok;
}

