#pragma once

#include "osd_shared.h"
#include "settings.h"

typedef enum FrameBlendState
{
    kFrameBlendState_Off,
    kFrameBlendState_On,
    kNumFrameBlendState,
} FrameBlendState_t;

OSD_Result_t FrameBlend_Draw(void* arg);
void FrameBlend_Update(const FrameBlendState_t eNewState);
OSD_Result_t FrameBlend_OnButton(const Button_t Button, const ButtonState_t State, void *arg);
OSD_Result_t FrameBlend_OnTransition(void* arg);
FrameBlendState_t FrameBlend_GetState(void);
OSD_Result_t FrameBlend_ApplySetting(SettingValue_t const *const pValue);
void FrameBlend_RegisterOnUpdateCb(fnOnUpdateCb_t fnOnUpdate);
