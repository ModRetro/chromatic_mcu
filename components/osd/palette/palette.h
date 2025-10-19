#pragma once

#include "style.h"

#include <stdint.h>

typedef enum PaletteType {
    kPalette_Bg,
    kPalette_Obj0,
    kPalette_Obj1
} PaletteType_t;

uint64_t Pal_GetColor(StyleID_t eID, PaletteType_t ePalT);
