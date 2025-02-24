#include "button.h"

#include "mutex.h"
#include "esp_log.h"

#include <stdint.h>
#include <stdbool.h>

enum {
    kMinHold_ticks = 10,

    kButtonBitsMask    = (1 << kNumButtons) - 1,
};

typedef struct ButtonCtx {
    ButtonState_t State;
} ButtonCtx_t;

__attribute__((unused)) static const char* TAG = "btn";

static ButtonCtx_t _Ctx[kNumButtons];
static const char* _ButtonNames[kNumButtons] = {
    [kButton_Start]  = "Button_Start",
    [kButton_Select] = "Button_Select",
    [kButton_B]      = "Button_B",
    [kButton_A]      = "Button_A",
    [kButton_Down]   = "Button_Down",
    [kButton_Right]  = "Button_Right",
    [kButton_Left]   = "Button_Left",
    [kButton_Up]     = "Button_Up",
    [kButton_MenuEn] = "Button_MenuEn",
    [kButton_MenuEnAlt] = "Button_MenuEnAlt",
};
static const char* _StateNames[kNumButtonStates] = {
    [kButtonState_None]     = "BState_None",
    [kButtonState_Pressed]  = "BState_Pressed",
};

static ButtonBits_t _PrevButtons = kButtonBits_None;

void Button_Update(uint16_t NewButtons)
{
    NewButtons = NewButtons & kButtonBitsMask;

    if (Mutex_Take(kMutexKey_Buttons) == kMutexResult_Ok)
    {
        for (Button_t b = kButton_Start; b < kNumButtons; b++)
        {
            const uint16_t mask = (1 << b);

            // Determine the button state based on previous and current state
            const uint16_t PrevState = _PrevButtons & mask;
            const uint16_t CurrState = NewButtons & mask;

            if (!PrevState && CurrState)
            {
                // Button was not pressed, now pressed
                _Ctx[b].State = kButtonState_Pressed;
            }
        }

        _PrevButtons = NewButtons;

        (void) Mutex_Give(kMutexKey_Buttons);
    }
}

void Button_ResetAll(void)
{
    if (Mutex_Take(kMutexKey_Buttons) == kMutexResult_Ok)
    {
        _PrevButtons = 0;
        for (Button_t b = kButton_Start; b < kNumButtons; b++)
        {
            _Ctx[b].State = kButtonState_None;
        }
        (void) Mutex_Give(kMutexKey_Buttons);
    }
}

ButtonState_t Button_GetState(const Button_t b)
{
    if (((unsigned)b >= kNumButtons) || (b == kButton_Select))
    {
        return kButtonState_None;
    }

    ButtonState_t s = kButtonState_None;

    if (Mutex_Take(kMutexKey_Buttons) == kMutexResult_Ok)
    {
        s = _Ctx[b].State;

        _Ctx[b].State = kButtonState_None;
        (void) Mutex_Give(kMutexKey_Buttons);
    }

    return s;
}

const char* Button_GetNameStr(const Button_t b)
{
    if (((unsigned)b >= kNumButtons))
    {
        return "unknown-btn";
    }

    return _ButtonNames[b];
}

const char* Button_GetStateStr(const ButtonState_t s)
{
    if ((unsigned)s >= kNumButtonStates)
    {
        return "unknown-btn-state";
    }

    return _StateNames[s];
}
