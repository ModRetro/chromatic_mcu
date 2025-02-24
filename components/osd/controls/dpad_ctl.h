#pragma once

#include "osd_shared.h"
#include "settings.h"

typedef enum DPadCtlState
{
    kDPadCtlState_AcceptDiag,
    kDPadCtlState_RejectDiag,
    kNumDPadCtlState,
} DPadCtlState_t;

OSD_Result_t DPadCtl_Draw(void* arg);
void DPadCtl_Update(const DPadCtlState_t eNewState);
OSD_Result_t DPadCtl_OnButton(const Button_t Button, const ButtonState_t State, void *arg);
OSD_Result_t DPadCtl_OnTransition(void* arg);
DPadCtlState_t DPadCtl_GetState(void);
OSD_Result_t DPadCtl_ApplySetting(SettingValue_t const *const pValue);
void DPadCtl_RegisterOnUpdateCb(fnOnUpdateCb_t fnOnUpdate);
