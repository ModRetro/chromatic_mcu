#include "palette.h"
#include "style.h"

#include <stdint.h>

enum {
    kPal_Clr3 = 48,
    kPal_Clr2 = 32,
    kPal_Clr1 = 16,

    // BasePalettes index for default/disabled state
    kPal_Default = 32,
};

// Create a 4-color palette from individual 16-bit RGB555 colors
#define PALETTE(c3, c2, c1, c0) \
    (((uint64_t)(c3) << kPal_Clr3) | ((uint64_t)(c2) << kPal_Clr2) | ((uint64_t)(c1) << kPal_Clr1) | (c0))

typedef struct {
    uint8_t obj0;
    uint8_t obj1;
    uint8_t bg;
} PaletteCombination_t;

// Palette combinations directly from cgb_boot.asm + extra for default
static const PaletteCombination_t PaletteCombinations[] = {
    {32, 32, 32}, // Default
    { 0,  0,  0}, // Brown      - Up
    { 4,  3, 28}, // Blue       - Left
    {12, 12, 12}, // Pastel     - Down
    {18, 18, 18}, // Green      - Right
    { 3, 28,  4}, // Red        - Up + A
    { 4,  0,  2}, // Dark Blue  - Left + A
    {24, 24, 24}, // Orange     - Down + A
    { 4,  4, 29}, // Dark Green - Right + A
    { 0,  0,  1}, // Dark Brown - Up + B
    { 5,  5,  5}, // Grayscale  - Left + B
    {28,  3,  6}, // Yellow     - Down + B
    {27, 27, 27}, // Negative   - Right + B
    {30, 30, 30}, // DMG1       - Right + A + B
    {31, 31, 31}, // DMG2       - Left + A + B
};

// Base palettes from cgb_boot.asm
// ASM format: Clr0, Clr1, Clr2, Clr3 (lightest to darkest)
static const uint64_t BasePalettes[] = {
    // Here:  Clr3    Clr2    Clr1    Clr0   (darkest to lightest)
    PALETTE(0x0000, 0x00D0, 0x32BF, 0x7FFF), //  0
    PALETTE(0x04CB, 0x15B0, 0x4279, 0x639F), //  1
    PALETTE(0x0000, 0x454A, 0x6E31, 0x7FFF), //  2
    PALETTE(0x0000, 0x0200, 0x1BEF, 0x7FFF), //  3
    PALETTE(0x0000, 0x1CF2, 0x421F, 0x7FFF), //  4
    PALETTE(0x0000, 0x294A, 0x5294, 0x7FFF), //  5
    PALETTE(0x0000, 0x012F, 0x03FF, 0x7FFF), //  6
    PALETTE(0x0000, 0x01D6, 0x03EF, 0x7FFF), //  7
    PALETTE(0x0000, 0x3DC8, 0x42B5, 0x7FFF), //  8
    PALETTE(0x0000, 0x0180, 0x03FF, 0x7E74), //  9
    PALETTE(0x2D6B, 0x1A13, 0x77AC, 0x67FF), // 10
    PALETTE(0x0000, 0x2175, 0x4BFF, 0x7ED6), // 11
    PALETTE(0x0000, 0x7E52, 0x4A5F, 0x53FF), // 12
    PALETTE(0x1CE0, 0x3A4C, 0x7ED2, 0x4FFF), // 13
    PALETTE(0x0000, 0x255F, 0x7FFF, 0x03ED), // 14
    PALETTE(0x7FFF, 0x03FF, 0x021F, 0x036A), // 15
    PALETTE(0x0000, 0x0112, 0x01DF, 0x7FFF), // 16
    PALETTE(0x0009, 0x00F2, 0x035F, 0x231F), // 17
    PALETTE(0x0000, 0x011F, 0x03EA, 0x7FFF), // 18
    PALETTE(0x0000, 0x000C, 0x001A, 0x299F), // 19
    PALETTE(0x0000, 0x001F, 0x027F, 0x7FFF), // 20
    PALETTE(0x0120, 0x0206, 0x03E0, 0x7FFF), // 21
    PALETTE(0x7C00, 0x001F, 0x7EEB, 0x7FFF), // 22
    PALETTE(0x001F, 0x7E00, 0x3FFF, 0x7FFF), // 23
    PALETTE(0x0000, 0x001F, 0x03FF, 0x7FFF), // 24
    PALETTE(0x0000, 0x000C, 0x001F, 0x03FF), // 25
    PALETTE(0x0000, 0x0193, 0x033F, 0x7FFF), // 26
    PALETTE(0x7FFF, 0x037F, 0x4200, 0x0000), // 27
    PALETTE(0x0000, 0x7C00, 0x7E8C, 0x7FFF), // 28
    PALETTE(0x0000, 0x6180, 0x1BEF, 0x7FFF), // 29
    PALETTE(0x1DC0, 0x2A60, 0x3B40, 0x43E0), // 30
    PALETTE(0x00E2, 0x0147, 0x018B, 0x01EF), // 31
    PALETTE(0x8000, 0x0000, 0x0000, 0x0000), // 32 - Default
};

uint64_t Pal_GetColor(StyleID_t eID, PaletteType_t ePalT)
{
    if (eID == kPalette_Default)
    {
        return BasePalettes[kPal_Default];
    }

    if ((unsigned)eID >= kNumPalettes)
    {
        return BasePalettes[kPal_Default];
    }

    uint8_t paletteIndex;
    switch (ePalT)
    {
        case kPalette_Bg:
            paletteIndex = PaletteCombinations[eID].bg;
            break;
        case kPalette_Obj0:
            paletteIndex = PaletteCombinations[eID].obj0;
            break;
        case kPalette_Obj1:
            paletteIndex = PaletteCombinations[eID].obj1;
            break;
        default:
            // Invalid palette type, return default
            return BasePalettes[kPal_Default];
    }

    return BasePalettes[paletteIndex];
}
