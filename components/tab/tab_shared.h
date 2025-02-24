#pragma once

#include "osd_shared.h"
#include "lvgl.h"

typedef struct TabItem {
    OSD_Widget_t Widget;
    lv_obj_t* DataObj;
} TabItem_t;

typedef struct TabCollection {
    sys_dlist_t WidgetList;
    TabItem_t* pCurrent;
    lv_obj_t* pDividerObj;
    lv_obj_t* pSelectedObj;
} TabCollection_t;

typedef struct Tab_DrawCtx {
    TabCollection_t* pTabList;
    void* pScreen;
    lv_color_t AccentColor;
} Tab_DrawCtx_t;

OSD_Result_t Tab_AddItem(TabCollection_t *const pList, TabItem_t *const pItem, lv_obj_t* pScreen);
OSD_Result_t Tab_OnButton(const Button_t Button, const ButtonState_t State, void *arg);
void Tab_Next(TabCollection_t *const pList);
void Tab_Prev(TabCollection_t *const pList);
