#pragma once

#include "dlist.h"
#include "lvgl.h"
#include "button.h"
#include <stdint.h>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

// As always, thank you https://stackoverflow.com/a/3437484
#ifndef MIN
#define MIN(a, b)               \
    ({                          \
    __typeof__ (a) _a = (a);    \
    __typeof__ (b) _b = (b);    \
     _a < _b ? _a : _b;         \
    })
#endif

#ifndef MAX
#define MAX(a, b)               \
    ({                          \
    __typeof__ (a) _a = (a);    \
    __typeof__ (b) _b = (b);    \
     _a > _b ? _a : _b;         \
    })
#endif

// Only ever append to the end of this error list! Do not re-arrange members to ensure error codes remain consistent
// across firmware releases.
typedef enum OSD_Result {
    kOSD_Result_Unknown,
    kOSD_Result_Ok,

    kOSD_Result_Err_Start = INT16_MIN,
    kOSD_Result_Err_NullDataPtr,
    kOSD_Result_Err_DuplicateWidget,
    kOSD_Result_Err_MenuAlreadyInit,
    kOSD_Result_Err_InvalidTabID,
    kOSD_Result_Err_MenuTabAlreadyInit,
    kOSD_Result_Err_SettingKeyInvalid,
    kOSD_Result_Err_SettingUpdateFailed,
    kOSD_Result_Err_SettingReadFailed,
    kOSD_Result_Err_SettingInvalidWidth,
    kOSD_Result_Err_SettingsNVSNotLoaded,
    kOSD_Result_Err_SettingsNVSAlreadyLoaded,
    kOSD_Result_Err_UnexpectedSettingDataType,
    kOSD_Result_Err_CmdRegFailed,
    kOSD_Result_Err_FailedToReadSN,

    kOSD_Result_Err_FirstUserErr,
} OSD_Result_t;

typedef void (*fnOnUpdateCb_t)(void);
typedef OSD_Result_t (*fnOSDCb_t)(void* arg);
typedef OSD_Result_t (*fnOSDButtonCb_t)(const Button_t Button, const ButtonState_t State, void *arg);

typedef struct OSD_Widget {
    // The node must not be relocated as we use this for menu rearrangement.
    sys_dnode_t Node;

    sys_dnode_t FocusNode;
    void* pFocusNodeArg;

    // Each menu defines a set of callbacks to initialize and draw itself.
    // User may override after adding the menu item.

    fnOSDCb_t fnDraw;
    fnOSDCb_t fnOnTransition;
    fnOSDButtonCb_t fnOnButton;

    // A human-friendly name for the widget specified by the user.
    const char* Name;
} OSD_Widget_t;

OSD_Result_t OSD_Common_Init(void);
lv_style_t* OSD_GetStyleTextWhite(void);
lv_style_t* OSD_GetStyleTextGrey(void);
lv_style_t* OSD_GetStyleTextBlack(void);
lv_style_t* OSD_GetStyleTextWhite_L(void);
