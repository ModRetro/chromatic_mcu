#include "button.h"

#include "mutex.h"
#include "esp_log.h"
#include "osd.h"
#include "esp_console.h"
#include "freertos/queue.h"

#include <stdint.h>
#include <stdbool.h>

enum {
    kMinHold_ticks = 10,

    kButtonBitsMask    = (1 << kNumButtons) - 1,

    kPokedInputsQLen = 10,
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

static fnOnButtonPokeCb_t _PokeHandler;
static StaticQueue_t _PokedInputsQueue;
static QueueHandle_t _PokedInputsHandle;
static ButtonBits_t _PokedInputBuffer[kPokedInputsQLen];
static ButtonBits_t _PrevButtons = kButtonBits_None;

static void Button_PokeInputs(const ButtonBits_t Inputs);
static int poke_button_command(int argc, char **argv);

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

static void Button_PokeInputs(const ButtonBits_t Inputs)
{
    if (_PokedInputsHandle == NULL)
    {
        _PokedInputsHandle = xQueueCreateStatic(kPokedInputsQLen, sizeof(_PokedInputBuffer[0]), (uint8_t*)_PokedInputBuffer, &_PokedInputsQueue);
        configASSERT(_PokedInputsHandle);
    }

    // Always forward menu buttons to the FPGA as it controls when the OSD is shown or hidden
    if ((Inputs & kButtonBits_MenuEnAlt) != 0 || (Inputs & kButtonBits_MenuEn) != 0)
    {
        if ((xQueueSend(_PokedInputsHandle, &Inputs, pdMS_TO_TICKS(1)) == pdPASS) && (_PokeHandler != NULL))
        {
            _PokeHandler();
        }
        return;
    }

    if (OSD_IsVisible())
    {
        // Update immediately
        Button_Update(Inputs);
    }
    else
    {
        if ((xQueueSend(_PokedInputsHandle, &Inputs, pdMS_TO_TICKS(1)) == pdPASS) && (_PokeHandler != NULL))
        {
            _PokeHandler();
        }
    }
}

uint16_t Button_GetPokedInputs(void)
{
    ButtonBits_t Input = 0;
    if (_PokedInputsHandle != NULL)
    {
        xQueueReceive(_PokedInputsHandle, &Input, 0);
    }
    return (uint16_t) Input;
}

void Button_RegisterOnButtonPokeCb(fnOnButtonPokeCb_t Handler)
{
    if (Handler == NULL)
    {
        ESP_LOGE(TAG, "Poke callback handler was null");
        return;
    }

    _PokeHandler = Handler;
}

static int poke_button_command(int argc, char **argv)
{
    if (argc != 2)
    {
        ESP_LOGE(TAG, "Poke expects a single argument");
        return -1;
    }

    char *arg0 = argv[1];
    if (strncmp(arg0, "0x", 2) == 0 ||
        strncmp(arg0, "0X", 2) == 0)
    {
        ESP_LOGE(TAG, "Argument must be hexadecimal without prefix.");
        return ESP_ERR_INVALID_ARG;
    }
    int32_t hex = strtol(arg0, NULL, 16);
    if (hex >= (1 << kNumButtons))
    {
        ESP_LOGE(TAG, "Illegal button bitmap.");
        return ESP_ERR_INVALID_ARG;
    }
    Button_PokeInputs((ButtonBits_t)hex);

    return ESP_OK;
}

void Button_RegisterCommands(void)
{
    esp_console_cmd_t command = {
        .command = "poke",
        .help = "Fakes a button input by the Chromatic",
        .func = &poke_button_command,
        .argtable = NULL,
    };

    esp_err_t err;
    if ((err = esp_console_cmd_register(&command)) != ESP_OK)
    {
        ESP_LOGE(TAG, "Registering '%s' command failed: %s", command.command, esp_err_to_name(err));
    }
}
