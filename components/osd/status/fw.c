#include "fw.h"

#include "osd_shared.h"
#include "lvgl.h"
#include "esp_log.h"

#if defined(ESP_PLATFORM)
#include "esp_console.h"
#include "esp_app_desc.h"
#endif

LV_IMG_DECLARE(img_chromatic);

enum {
    kMaxVerLen = 16,

    kTextX_px = 80,
    kTextY_px = 45,

    kTextFWVerY_px   = kTextY_px + 20,
    kTextFPGAVerY_px = kTextFWVerY_px + 23,
};

static const lv_point_t MessageOrigin = {.x = kTextX_px, .y = kTextY_px};
static const lv_point_t FWVerOrigin = {.x = kTextX_px, .y = kTextFWVerY_px };
static const lv_point_t FPGAVerOrigin = {.x = kTextX_px, .y = kTextFPGAVerY_px };

typedef struct Firmware {
    lv_obj_t* pMessageObj;
    lv_obj_t* pFWVersionObj;
    lv_obj_t* pFPGAVersionObj;
    lv_obj_t* pChromaticImgObj;
    char FWVerStr[kMaxVerLen];
    char FPGAVerStr[kMaxVerLen];
    uint8_t FPGAVersionMajor;       // 0 means unknown or not supported by the connected FPGA
    uint8_t FPGAVersionMinor;       // 0 means unknown or not supported by the connected FPGA
    uint8_t FPGA_IsDebugBuild;      // 0 means unknown or not supported by the connected FPGA
} Firmware_t;

static Firmware_t _Ctx;
static const char* TAG = "fw";

#if defined(ESP_PLATFORM)
static int version_command(int argc, char **argv);
static OSD_Result_t register_version_dump(void);
#endif

OSD_Result_t Firmware_Draw(void* arg)
{
    if (arg == NULL)
    {
        return kOSD_Result_Err_NullDataPtr;
    }

    lv_obj_t *const pScreen = (lv_obj_t *const) arg;

    if (_Ctx.pMessageObj == NULL)
    {
        _Ctx.pMessageObj = lv_label_create(pScreen);
        // Print the firmware message
        lv_obj_add_style(_Ctx.pMessageObj, OSD_GetStyleTextGrey(), 0);
        lv_obj_align(_Ctx.pMessageObj, LV_ALIGN_TOP_LEFT, MessageOrigin.x, MessageOrigin.y);
        lv_label_set_text(_Ctx.pMessageObj, "THIS DEVICE IS\nRUNNING FW:\n\n\nFPGA:");
    }
    else
    {
        lv_obj_move_foreground(_Ctx.pMessageObj);
    }

    if (_Ctx.pFWVersionObj == NULL)
    {
        _Ctx.pFWVersionObj = lv_label_create(pScreen);

        // Print the firmware version
        lv_obj_add_style(_Ctx.pFWVersionObj, OSD_GetStyleTextWhite(), 0);
        lv_obj_align(_Ctx.pFWVersionObj, LV_ALIGN_TOP_LEFT, FWVerOrigin.x, FWVerOrigin.y);
        lv_label_set_text(_Ctx.pFWVersionObj, (const char*)_Ctx.FWVerStr);
    }
    else
    {
        lv_obj_move_foreground(_Ctx.pFWVersionObj);
    }

    if (_Ctx.pFPGAVersionObj == NULL)
    {
        _Ctx.pFPGAVersionObj = lv_label_create(pScreen);

        lv_snprintf(
            _Ctx.FPGAVerStr, sizeof(_Ctx.FPGAVerStr),
            "v%u.%u%s",
            _Ctx.FPGAVersionMajor,
            _Ctx.FPGAVersionMinor,
            _Ctx.FPGA_IsDebugBuild ? " (dbg)" : " "
        );

        // Print the firmware version
        lv_obj_add_style(_Ctx.pFPGAVersionObj, OSD_GetStyleTextWhite(), 0);
        lv_obj_align(_Ctx.pFPGAVersionObj, LV_ALIGN_TOP_LEFT, FPGAVerOrigin.x, FPGAVerOrigin.y);
        lv_label_set_text(_Ctx.pFPGAVersionObj, (const char*)_Ctx.FPGAVerStr);
    }
    else
    {
        lv_obj_move_foreground(_Ctx.pFPGAVersionObj);
    }

    if (_Ctx.pChromaticImgObj == NULL)
    {
        _Ctx.pChromaticImgObj = lv_img_create(pScreen);
        lv_obj_align(_Ctx.pChromaticImgObj, LV_ALIGN_BOTTOM_RIGHT, -17, -38);
    }
    else
    {
        // Draw the brightness image indicating no brightness
        lv_img_set_src(_Ctx.pChromaticImgObj, &img_chromatic);
    }

    return kOSD_Result_Ok;
}

OSD_Result_t Firmware_OnTransition(void* arg)
{
    (void)(arg);

    const lv_obj_t** ToDelete[] = {
        (const lv_obj_t**)&_Ctx.pChromaticImgObj,
        (const lv_obj_t**)&_Ctx.pMessageObj,
        (const lv_obj_t**)&_Ctx.pFWVersionObj,
        (const lv_obj_t**)&_Ctx.pFPGAVersionObj,
    };

    for (size_t i = 0; i < ARRAY_SIZE(ToDelete); i++)
    {
        if (*ToDelete[i] != NULL)
        {
            lv_obj_del((lv_obj_t*)*ToDelete[i]);
            *(ToDelete[i]) = NULL;
        }
    }

    return kOSD_Result_Ok;
}

void Firmware_SetFPGAVersion(uint8_t VersionMajor, uint8_t VersionMinor, uint8_t IsDebug)
{
    _Ctx.FPGAVersionMajor = VersionMajor;
    _Ctx.FPGAVersionMinor = VersionMinor;
    _Ctx.FPGA_IsDebugBuild = IsDebug;
}

OSD_Result_t Firmware_Initialize(void)
{
    OSD_Result_t eResult = kOSD_Result_Ok;

#if defined(ESP_PLATFORM)
    const esp_app_desc_t* pDesc = esp_app_get_description();
    lv_snprintf(_Ctx.FWVerStr, sizeof(_Ctx.FWVerStr), "v%s", pDesc->version);
    eResult = register_version_dump();
#else
    lv_snprintf(_Ctx.FWVerStr, sizeof(_Ctx.FWVerStr), "v%s", "1.00-SIM");
#endif

    return eResult;
}

#if defined(ESP_PLATFORM)
static int version_command(int argc, char **argv)
{
    const esp_app_desc_t* pDesc = esp_app_get_description();
    lv_snprintf(_Ctx.FWVerStr, sizeof(_Ctx.FWVerStr), "v%s", pDesc->version);

    printf(
        "{ \"mcu\": \"%s\", \"fpga\": \"%d.%d\" }\r\n",
        _Ctx.FWVerStr,
        _Ctx.FPGAVersionMajor,
        _Ctx.FPGAVersionMinor
    );

    return 0;
}

static OSD_Result_t register_version_dump(void)
{
    esp_console_cmd_t command = {
        .command = "fwversion",
        .help = "Print the device version information",
        .func = &version_command,
        .argtable = NULL,
    };

    esp_err_t err;
    if ((err = esp_console_cmd_register(&command)) != ESP_OK)
    {
        ESP_LOGE(TAG, "Registering 'version' command failed: %s", esp_err_to_name(err));
        return kOSD_Result_Err_CmdRegFailed;
    }

    return kOSD_Result_Ok;
}
#endif
