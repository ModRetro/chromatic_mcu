#pragma once

#include <stdbool.h>
#include <stdint.h>

// Bit definitions follow the scheme sent to us by the FPGA
typedef enum Button {
    kButton_Start,
    kButton_Select,
    kButton_B,
    kButton_A,
    kButton_Up,
    kButton_Right,
    kButton_Left,
    kButton_Down,
    kButton_MenuEnAlt,
    kButton_MenuEn,
    kNumButtons,
    kButton_None,
} Button_t;

typedef enum ButtonState {
    kButtonState_None,
    kButtonState_Pressed,
    kNumButtonStates,
} ButtonState_t;

typedef enum ButtonBits {
    kButtonBits_None   = 0,
    kButtonBits_B      = (1 << kButton_B),
    kButtonBits_A      = (1 << kButton_A),
    kButtonBits_Down   = (1 << kButton_Down),
    kButtonBits_Right  = (1 << kButton_Right),
    kButtonBits_Left   = (1 << kButton_Left),
    kButtonBits_Up     = (1 << kButton_Up),
    kButtonBits_MenuEn = (1 << kButton_MenuEn),
    kButtonBits_MenuEnAlt = (1 << kButton_MenuEnAlt),
} ButtonBits_t;

typedef void (*fnOnButtonPokeCb_t)(void);

void Button_Update(const uint16_t NewButtons);
ButtonState_t Button_GetState(const Button_t b);
const char* Button_GetNameStr(const Button_t b);
const char* Button_GetStateStr(const ButtonState_t s);
void Button_ResetAll(void);
void Button_RegisterOnButtonPokeCb(fnOnButtonPokeCb_t Handler);
uint16_t Button_GetPokedInputs(void);
void Button_RegisterCommands(void);
