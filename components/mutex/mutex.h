#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef enum MutexResult
{
    kMutexResult_Ok,

    kMutexResult_Err_Timeout = -127,
    kMutexResult_Err_InvalidKey,
    kMutexResult_Err_UndersizedOutput,
    kMutexResult_Err_InvalidDataPtr,
    kMutexResult_Err_ResourceWasNotAcquired,
} MutexResult_t;

typedef enum MutexKey
{
    kMutexKey_Buttons,
    kMutexKey_Battery,
    kMutexKey_DPadCtl,
    kMutexKey_ColorCorrectUSB,
    kMutexKey_SilentMode,
    kMutexKey_FrameBlend,
    kMutexKey_LowBattIconCtl,
    kMutexKey_ScreenTransitCtl,
    kMutexKey_Brightness,
    kMutexKey_PlayerNum,
    kNumMutexKeys,

    kMutexKey_FirstKey = kMutexKey_Buttons,
} MutexKey_t;

void Mutex_Init(void);
MutexResult_t Mutex_Take(const MutexKey_t eKey);
MutexResult_t Mutex_Give(const MutexKey_t eKey);
