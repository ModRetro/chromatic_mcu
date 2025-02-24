#pragma once

#include "osd_shared.h"

#include <stdint.h>

// The pixel number in horizontal and vertical
#ifndef DISP_W_RES_PX
#define DISP_W_RES_PX 160u
#endif

#ifndef DISP_H_RES_PX
#define DISP_H_RES_PX 144u
#endif

enum {
    kOSD_Width_px  = DISP_W_RES_PX,
    kOSD_Height_px = DISP_H_RES_PX,
    kOSD_NumPixels = kOSD_Height_px * kOSD_Width_px,
};

OSD_Result_t OSD_Initialize(void);
OSD_Result_t OSD_AddWidget(OSD_Widget_t *const pWidget);
void OSD_Draw(void* arg);
void OSD_HandleInputs(void);
bool OSD_IsVisible(void);
void OSD_SetVisiblityState(const bool IsVisible);
