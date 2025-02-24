#include "tab_shared.h"

#include "dlist.h"
#include "osd_shared.h"

#include "lvgl.h"
#include "esp_log.h"

#include <stdint.h>

static const char* TAG = "TabShared";

OSD_Result_t Tab_AddItem(TabCollection_t *const pList, TabItem_t *const pItem, lv_obj_t* pScreen)
{
    if ((pList == NULL) || ( pItem == NULL ) || (pScreen == NULL))
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    // Confirm the node does not already exist in the list
    sys_dnode_t* pNode = NULL;
    SYS_DLIST_FOR_EACH_NODE(&pList->WidgetList, pNode) {
        if (pNode == NULL)
        {
            // List is empty or we are at the tail
            break;
        }
        else if (pNode == &pItem->Widget.Node)
        {
            return kOSD_Result_Err_DuplicateWidget;
        }
    }

    if (sys_dlist_is_empty(&pList->WidgetList))
    {
        // Set the current item to the very first
        pList->pCurrent = pItem;
    }

    sys_dnode_init(&pItem->Widget.Node);
    sys_dlist_append( &pList->WidgetList, &pItem->Widget.Node );

    ESP_LOGI(TAG, "Added menu item %s", pItem->Widget.Name);

    return kOSD_Result_Ok;
}

OSD_Result_t Tab_OnButton(const Button_t Button, const ButtonState_t State, void *arg)
{
    if (arg == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    TabCollection_t* pList = (TabCollection_t*)(arg);

    if (sys_dlist_is_empty(&pList->WidgetList))
    {
        ESP_LOGW(TAG, "tab list is empty");
        return kOSD_Result_Ok;
    }

    switch (Button)
    {
        // Fall-through
        case kButton_A:
        case kButton_B:
            if (State == kButtonState_Pressed)
            {
                if (pList->pCurrent->Widget.fnOnButton != NULL)
                {
                    pList->pCurrent->Widget.fnOnButton(Button, State, arg);
                }
            }
            break;
        default:
            break;
    }
    return kOSD_Result_Ok;
}

void Tab_Next(TabCollection_t *const pList)
{
    if (pList == NULL)
    {
        return;
    }

    sys_dnode_t* pNode = &pList->pCurrent->Widget.Node;
    TabItem_t* pNextItem = (TabItem_t*)sys_dlist_peek_next(&pList->WidgetList, pNode);
    if (pNextItem != NULL)
    {
        OSD_Widget_t* pWidget = (OSD_Widget_t*)pNode;
        ESP_LOGI(TAG, "Widget in list: %s Next: %s", pWidget->Name, pNextItem->Widget.Name);

        if (pList->pCurrent->Widget.fnOnTransition != NULL)
        {
            pList->pCurrent->Widget.fnOnTransition(NULL);
        }

        if (pList->pSelectedObj != NULL)
        {
            lv_obj_del(pList->pSelectedObj);
            pList->pSelectedObj = NULL;
        }

        pList->pCurrent = pNextItem;
    }
}

void Tab_Prev(TabCollection_t *const pList)
{
    if (pList == NULL)
    {
        return;
    }

    sys_dnode_t* pNode = &pList->pCurrent->Widget.Node;
    TabItem_t* pPrevItem = (TabItem_t*)sys_dlist_peek_prev(&pList->WidgetList, pNode);
    if (pPrevItem != NULL)
    {
        OSD_Widget_t* pWidget=(OSD_Widget_t*)pNode;
        ESP_LOGI(TAG, "Widget in list: %s Next: %s", pWidget->Name, pPrevItem->Widget.Name);

        if (pList->pCurrent->Widget.fnOnTransition != NULL)
        {
            pList->pCurrent->Widget.fnOnTransition(NULL);
        }

        if (pList->pSelectedObj != NULL)
        {
            lv_obj_del(pList->pSelectedObj);
            pList->pSelectedObj = NULL;
        }

        pList->pCurrent = pPrevItem;
    }
}
