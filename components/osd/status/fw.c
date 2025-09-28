#include "fw.h"

#include "osd_shared.h"
#include "lvgl.h"
#include "esp_log.h"

#if defined(ESP_PLATFORM)
#include "esp_console.h"
#include "esp_app_desc.h"
#endif

#ifndef CONFIG_CHROMATIC_FW_VER_STR
#error "The top-level Chromatic firmware version string must be defined."
#endif

LV_IMG_DECLARE(img_chromatic);

enum {
    kMaxVerLen = 16,
    kTextH_px  = 5,

    kOriginX_px = 73,  // top left corner of right section
    kOriginY_px = 38,

    kHeaderPadX_px = 4,
    kHeaderPadY_px = 6,

    kDetailsPadX_px = kHeaderPadX_px,
    kDetailsPadY_px = kHeaderPadY_px + kTextH_px + 6,
    kDetailsH_px    = 18,
    kDetailsW_px    = 65,

    kChromaticX_px = -17,  // Relative to bottom right
    kChromaticY_px = -38,

    kTextPadY_px  = kTextH_px + 3,  // Offset between lines of text
    kTitlePadX_px = 20              // Offset for title vs version text
};

static const lv_point_t HeaderOrigin    = {.x = kOriginX_px + kHeaderPadX_px, .y = kOriginY_px + kHeaderPadY_px};
static const lv_point_t DetailsOrigin   = {.x = kOriginX_px + kDetailsPadX_px, .y = kOriginY_px + kDetailsPadY_px};
static const lv_point_t ChromaticOrigin = {.x = kChromaticX_px, .y = kChromaticY_px};
static const lv_point_t FWTitleOrigin   = {.x = 0, .y = 0 };  // Relative to DetailsOrigin
static const lv_point_t FWVerOrigin     = {.x = FWTitleOrigin.x + kTitlePadX_px, .y = FWTitleOrigin.y };
static const lv_point_t MCUTitleOrigin  = {.x = 0, .y = 0 };
static const lv_point_t MCUVerOrigin    = {.x = MCUTitleOrigin.x + kTitlePadX_px, .y = MCUTitleOrigin.y };
static const lv_point_t FPGATitleOrigin = {.x = 0, .y = MCUVerOrigin.y + kTextPadY_px };
static const lv_point_t FPGAVerOrigin   = {.x = FPGATitleOrigin.x + kTitlePadX_px, .y = FPGATitleOrigin.y };
static const lv_point_t DetailsSize     = {.x = kDetailsW_px, .y = kDetailsH_px};

typedef struct Firmware {
    lv_obj_t* pHeaderObj;
    lv_obj_t* pDetailsObj;
    lv_obj_t* pChromaticImgObj;
    /* Cached child objects inside pDetailsObj to avoid recreating on every draw */
    lv_obj_t* pMCUTitle;
    lv_obj_t* pMCUInfo;
    lv_obj_t* pFPGATitle;
    lv_obj_t* pFPGAInfo;
    lv_obj_t* pFWTitle;
    lv_obj_t* pFWInfo;
    char ChromaticFWVerStr[kMaxVerLen];
    char AppFWVerStr[kMaxVerLen];
    char FPGAVerStr[kMaxVerLen];
    uint8_t FPGAVersionMajor;       // 0 means unknown or not supported by the connected FPGA
    uint8_t FPGAVersionMinor;       // 0 means unknown or not supported by the connected FPGA
    uint8_t FPGA_IsDebugBuild;      // 0 means unknown or not supported by the connected FPGA
    bool ShowDetails;
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

    if (_Ctx.pHeaderObj == NULL)
    {
        _Ctx.pHeaderObj = lv_label_create(pScreen);
        lv_label_set_text(_Ctx.pHeaderObj, "SYSTEM    INFO");
        lv_obj_align(_Ctx.pHeaderObj, LV_ALIGN_TOP_LEFT, HeaderOrigin.x, HeaderOrigin.y);
        lv_obj_add_style(_Ctx.pHeaderObj, OSD_GetStyleTextGrey(), 0);
    }

    if (_Ctx.pDetailsObj == NULL)
    {
        _Ctx.pDetailsObj = lv_obj_create(pScreen);
        lv_obj_remove_style_all(_Ctx.pDetailsObj);
        lv_obj_align(_Ctx.pDetailsObj, LV_ALIGN_TOP_LEFT, DetailsOrigin.x, DetailsOrigin.y);
        lv_obj_set_size(_Ctx.pDetailsObj, DetailsSize.x, DetailsSize.y);
    }

    if (_Ctx.pChromaticImgObj == NULL)
    {
        _Ctx.pChromaticImgObj = lv_img_create(pScreen);
        lv_img_set_src(_Ctx.pChromaticImgObj, &img_chromatic);
        lv_obj_align(_Ctx.pChromaticImgObj, LV_ALIGN_BOTTOM_RIGHT, ChromaticOrigin.x, ChromaticOrigin.y);
    }

    if (_Ctx.ShowDetails)
    {
        // MCU Text
        if (_Ctx.pMCUTitle == NULL) 
        {
            _Ctx.pMCUTitle = lv_label_create(_Ctx.pDetailsObj);
            lv_label_set_text(_Ctx.pMCUTitle, "MCU:");
            lv_obj_align(_Ctx.pMCUTitle, LV_ALIGN_TOP_LEFT, MCUTitleOrigin.x, MCUTitleOrigin.y);
            lv_obj_add_style(_Ctx.pMCUTitle, OSD_GetStyleTextGrey(), LV_PART_MAIN);
        }

        // MCU Version
        if (_Ctx.pMCUInfo == NULL) 
        {
            _Ctx.pMCUInfo = lv_label_create(_Ctx.pDetailsObj);
            lv_label_set_text(_Ctx.pMCUInfo, (const char*)_Ctx.AppFWVerStr);
            lv_obj_align(_Ctx.pMCUInfo, LV_ALIGN_TOP_LEFT, MCUVerOrigin.x, MCUVerOrigin.y);
            lv_obj_add_style(_Ctx.pMCUInfo, OSD_GetStyleTextWhite(), LV_PART_MAIN);
        }

        // FPGA Text
        if (_Ctx.pFPGATitle == NULL) 
        {
            _Ctx.pFPGATitle = lv_label_create(_Ctx.pDetailsObj);
            lv_label_set_text(_Ctx.pFPGATitle, "FPGA:");
            lv_obj_align(_Ctx.pFPGATitle, LV_ALIGN_TOP_LEFT, FPGATitleOrigin.x, FPGATitleOrigin.y);
            lv_obj_add_style(_Ctx.pFPGATitle, OSD_GetStyleTextGrey(), LV_PART_MAIN);
        }

        // FPGA Version
        if (_Ctx.pFPGAInfo == NULL) 
        {
            lv_snprintf(
                _Ctx.FPGAVerStr, sizeof(_Ctx.FPGAVerStr),
                "v%u.%u%s",
                _Ctx.FPGAVersionMajor,
                _Ctx.FPGAVersionMinor,
                _Ctx.FPGA_IsDebugBuild ? " (dbg)" : " "
            );
            _Ctx.pFPGAInfo = lv_label_create(_Ctx.pDetailsObj);
            lv_label_set_text(_Ctx.pFPGAInfo, (const char*)_Ctx.FPGAVerStr);
            lv_obj_align(_Ctx.pFPGAInfo, LV_ALIGN_TOP_LEFT, FPGAVerOrigin.x, FPGAVerOrigin.y);
            lv_obj_add_style(_Ctx.pFPGAInfo, OSD_GetStyleTextWhite(), LV_PART_MAIN);
        }
    }
    else 
    {
        // App FW Text
        if (_Ctx.pFWTitle == NULL) 
        {
            _Ctx.pFWTitle = lv_label_create(_Ctx.pDetailsObj);
            lv_label_set_text(_Ctx.pFWTitle, "FW:");
            lv_obj_align(_Ctx.pFWTitle, LV_ALIGN_TOP_LEFT, FWTitleOrigin.x, FWTitleOrigin.y);
            lv_obj_add_style(_Ctx.pFWTitle, OSD_GetStyleTextGrey(), LV_PART_MAIN);
        }

        // App FW Version
        if (_Ctx.pFWInfo == NULL) 
        {
            _Ctx.pFWInfo = lv_label_create(_Ctx.pDetailsObj);
            lv_label_set_text(_Ctx.pFWInfo, (const char*)_Ctx.ChromaticFWVerStr);
            lv_obj_align(_Ctx.pFWInfo, LV_ALIGN_TOP_LEFT, FWVerOrigin.x, FWVerOrigin.y);
            lv_obj_add_style(_Ctx.pFWInfo, OSD_GetStyleTextWhite(), LV_PART_MAIN);
        }
    }

    return kOSD_Result_Ok;
}

OSD_Result_t Firmware_OnTransition(void* arg)
{
    (void)(arg);

    const lv_obj_t** ToDelete[] = {
        (const lv_obj_t**)&_Ctx.pHeaderObj,
        (const lv_obj_t**)&_Ctx.pDetailsObj,
        (const lv_obj_t**)&_Ctx.pChromaticImgObj,
        // Children of pDetailsObj
        (const lv_obj_t**)&_Ctx.pMCUTitle,
        (const lv_obj_t**)&_Ctx.pMCUInfo,
        (const lv_obj_t**)&_Ctx.pFPGATitle,
        (const lv_obj_t**)&_Ctx.pFPGAInfo,
        (const lv_obj_t**)&_Ctx.pFWTitle,
        (const lv_obj_t**)&_Ctx.pFWInfo
    };

    for (size_t i = 0; i < ARRAY_SIZE(ToDelete); i++)
    {
        if (*ToDelete[i] != NULL)
        {
            // LVGL children objs will still have an address, but don't exist inside
            // hierarchy, so they need to be double checked
            if (lv_obj_is_valid((lv_obj_t*)*ToDelete[i]))
            {
                lv_obj_del((lv_obj_t*)*ToDelete[i]);
            }
            *(ToDelete[i]) = NULL;
        }
    }

    return kOSD_Result_Ok;
}

OSD_Result_t Firmware_OnButton(const Button_t Button, const ButtonState_t State, void *arg)
{
    (void)arg;

    switch (Button)
    {
        case kButton_A:
        case kButton_B:
        {
            if (State == kButtonState_Pressed)
            {
                _Ctx.ShowDetails = !_Ctx.ShowDetails;

                // Clear cached objects, so details get redrawn correctly
                Firmware_OnTransition(NULL);
            }
            break;
        }
        default:
            break;
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
    lv_snprintf(_Ctx.AppFWVerStr, sizeof(_Ctx.AppFWVerStr), "v%s", pDesc->version);
    lv_snprintf(_Ctx.ChromaticFWVerStr, sizeof(_Ctx.ChromaticFWVerStr), "v%s", CONFIG_CHROMATIC_FW_VER_STR);
    eResult = register_version_dump();
#else
    lv_snprintf(_Ctx.AppFWVerStr, sizeof(_Ctx.AppFWVerStr), "v%s", "1.00-SIM");
    lv_snprintf(_Ctx.ChromaticFWVerStr, sizeof(_Ctx.ChromaticFWVerStr), "v%s", "1.0-SIM");
#endif

    return eResult;
}

#if defined(ESP_PLATFORM)
static int version_command(int argc, char **argv)
{
    const esp_app_desc_t* pDesc = esp_app_get_description();
    lv_snprintf(_Ctx.AppFWVerStr, sizeof(_Ctx.AppFWVerStr), "v%s", pDesc->version);

    printf(
        "{ \"mcu\": \"%s\", \"fpga\": \"%d.%d\", \"chromatic\": \"%s\" }\r\n",
        _Ctx.AppFWVerStr,
        _Ctx.FPGAVersionMajor,
        _Ctx.FPGAVersionMinor,
        _Ctx.ChromaticFWVerStr
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
