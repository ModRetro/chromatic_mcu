#pragma once

#include "osd_shared.h"

OSD_Result_t Firmware_Draw(void* arg);
OSD_Result_t Firmware_OnTransition(void* arg);
void Firmware_SetFPGAVersion(uint8_t VersionMajor, uint8_t VersionMinor, uint8_t IsDebug);
OSD_Result_t Firmware_Initialize(void);
OSD_Result_t Firmware_OnButton(const Button_t Button, const ButtonState_t State, void *arg);
