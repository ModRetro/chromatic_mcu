#include "osd.h"

#include "button.h"
#include "color.h"
#include "dlist.h"
#include "osd_shared.h"

#include "esp_log.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef struct OSD {
    sys_dlist_t WidgetList;
    bool IsVisible;
} OSD_t;

static const char* TAG = "OSD";
static OSD_t OSD;

OSD_Result_t OSD_Initialize(void)
{
    memset(&OSD, 0x0, sizeof(OSD));

    sys_dlist_init(&OSD.WidgetList);

    OSD_Common_Init();

    return kOSD_Result_Ok;
}

OSD_Result_t OSD_AddWidget( OSD_Widget_t *const pWidget )
{
    if ( pWidget == NULL )
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    // Confirm the node does not already exist in the list
    sys_dnode_t* pNode = NULL;
    SYS_DLIST_FOR_EACH_NODE(&OSD.WidgetList, pNode) {
        if (pNode == NULL)
        {
            // List is empty or we are at the tail
            break;
        }
        else if (pNode == &pWidget->Node)
        {
            return kOSD_Result_Err_DuplicateWidget;
        }
    }

    sys_dnode_init(&pWidget->Node);
    sys_dnode_init(&pWidget->FocusNode);

    sys_dlist_append( &OSD.WidgetList, &pWidget->Node );

    ESP_LOGI(TAG, "Added widget %s", pWidget->Name);

    return kOSD_Result_Ok;
}

void OSD_Draw(void* arg)
{
    sys_dnode_t* pNode = NULL;
    SYS_DLIST_FOR_EACH_NODE(&OSD.WidgetList, pNode) {
        if (pNode == NULL)
        {
            // List is empty or we are at the tail
            break;
        }
        else
        {
            OSD_Widget_t *const pWidget = (OSD_Widget_t*)pNode;

            if (pWidget->fnDraw != NULL)
            {
                OSD_Result_t eResult = pWidget->fnDraw(arg);
                if (eResult != kOSD_Result_Ok)
                {
                    ESP_LOGD(TAG, "Error %d during %s draw", eResult, pWidget->Name);
                }
            }
        }
    }
}

void OSD_HandleInputs(void)
{
    sys_dnode_t* pNode = NULL;

    SYS_DLIST_FOR_EACH_NODE(&OSD.WidgetList, pNode)
    {
        const OSD_Widget_t *const pWidget = (OSD_Widget_t*)(pNode);
        if ((pWidget != NULL) && (pWidget->fnOnButton != NULL))
        {
            // FIXME: This needs to change for multi-button input handling.
            // Right now it only does one at a time.
            for (size_t i = 0; i < kNumButtons; i++)
            {
                const Button_t b = (Button_t)i;
                const ButtonState_t s = Button_GetState(b);

                if (s != kButtonState_None)
                {
                    //ESP_LOGD(TAG, "%s is %s", Button_GetNameStr(b), Button_GetStateStr(s));

                    const OSD_Result_t eResult = pWidget->fnOnButton(b, s, pWidget->pFocusNodeArg);

                    if (eResult != kOSD_Result_Ok)
                    {
                        ESP_LOGE(TAG, "%s onButton failed with %d", pWidget->Name, eResult);
                    }
                }
            }
        }
    }
}

bool OSD_IsVisible(void)
{
    return OSD.IsVisible;
}

void OSD_SetVisiblityState(const bool IsVisible)
{
    OSD.IsVisible = IsVisible;
}
