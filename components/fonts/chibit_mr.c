/*******************************************************************************
 * Size: 8 px
 * Bpp: 1
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef CHIBIT_MR
#define CHIBIT_MR 1
#endif

#if CHIBIT_MR

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xf4,

    /* U+0022 "\"" */
    0xb4,

    /* U+0023 "#" */
    0x57, 0xd5, 0xf5, 0x0,

    /* U+0024 "$" */
    0x55, 0x62, 0xa2, 0x0,

    /* U+0025 "%" */
    0x18, 0x51, 0x5c, 0xf1, 0x80,

    /* U+0026 "&" */
    0x31, 0x28, 0x61, 0x48, 0xc0,

    /* U+0027 "'" */
    0xc0,

    /* U+0028 "(" */
    0x6a, 0x90,

    /* U+0029 ")" */
    0x95, 0x60,

    /* U+002A "*" */
    0x27, 0xdc, 0xa0,

    /* U+002B "+" */
    0x21, 0x3e, 0x42, 0x0,

    /* U+002C "," */
    0xc0,

    /* U+002D "-" */
    0xe0,

    /* U+002E "." */
    0x80,

    /* U+002F "/" */
    0x25, 0x29, 0x0,

    /* U+0030 "0" */
    0x69, 0x99, 0x96,

    /* U+0031 "1" */
    0x75, 0x50,

    /* U+0032 "2" */
    0x69, 0x12, 0x4f,

    /* U+0033 "3" */
    0x69, 0x31, 0x96,

    /* U+0034 "4" */
    0x26, 0xaf, 0x22,

    /* U+0035 "5" */
    0xf8, 0xe1, 0x96,

    /* U+0036 "6" */
    0x68, 0xe9, 0x96,

    /* U+0037 "7" */
    0xf9, 0x22, 0x44,

    /* U+0038 "8" */
    0x69, 0x69, 0x96,

    /* U+0039 "9" */
    0x69, 0x97, 0x16,

    /* U+003A ":" */
    0x90,

    /* U+003B ";" */
    0x98,

    /* U+003C "<" */
    0x2a, 0x22,

    /* U+003D "=" */
    0xf0, 0xf0,

    /* U+003E ">" */
    0x88, 0xa8,

    /* U+003F "?" */
    0x69, 0x32, 0x2,

    /* U+0040 "@" */
    0x7a, 0x19, 0xe9, 0x99, 0x0,

    /* U+0041 "A" */
    0x22, 0xa3, 0xf8, 0xc4,

    /* U+0042 "B" */
    0xf4, 0x7d, 0x18, 0xf8,

    /* U+0043 "C" */
    0x74, 0x61, 0x8, 0xb8,

    /* U+0044 "D" */
    0xe4, 0xa3, 0x19, 0x70,

    /* U+0045 "E" */
    0xfc, 0x79, 0x8, 0xfc,

    /* U+0046 "F" */
    0xfc, 0x61, 0xe8, 0x40,

    /* U+0047 "G" */
    0x74, 0x61, 0x78, 0xb8,

    /* U+0048 "H" */
    0x8c, 0x7f, 0x18, 0xc4,

    /* U+0049 "I" */
    0xe9, 0x25, 0xc0,

    /* U+004A "J" */
    0xf8, 0x85, 0x29, 0x30,

    /* U+004B "K" */
    0x8c, 0x7d, 0x28, 0xc4,

    /* U+004C "L" */
    0x84, 0x21, 0x8, 0xfc,

    /* U+004D "M" */
    0x8e, 0xeb, 0x18, 0xc4,

    /* U+004E "N" */
    0x8e, 0x6b, 0x59, 0xc4,

    /* U+004F "O" */
    0x74, 0x63, 0x18, 0xb8,

    /* U+0050 "P" */
    0xf4, 0x63, 0xe8, 0x40,

    /* U+0051 "Q" */
    0x72, 0x28, 0xae, 0xc9, 0xd0,

    /* U+0052 "R" */
    0xf4, 0x63, 0xe9, 0x44,

    /* U+0053 "S" */
    0x74, 0x7c, 0x18, 0xb8,

    /* U+0054 "T" */
    0xfd, 0x48, 0x42, 0x38,

    /* U+0055 "U" */
    0xee, 0x89, 0x12, 0x24, 0x47, 0x0,

    /* U+0056 "V" */
    0xee, 0x89, 0x11, 0x42, 0x82, 0x0,

    /* U+0057 "W" */
    0x8c, 0x63, 0x5d, 0xc4,

    /* U+0058 "X" */
    0x99, 0x69, 0x99,

    /* U+0059 "Y" */
    0x8c, 0x54, 0x42, 0x10,

    /* U+005A "Z" */
    0xf8, 0x44, 0x44, 0xfc,

    /* U+005B "[" */
    0xea, 0xb0,

    /* U+005C "\\" */
    0x8a, 0xbe, 0x4f, 0x90,

    /* U+005D "]" */
    0xd5, 0x70,

    /* U+005E "^" */
    0x22, 0xa2,

    /* U+005F "_" */
    0xe0,

    /* U+0060 "`" */
    0x90,

    /* U+0061 "a" */
    0x64, 0x9d, 0x27, 0x80,

    /* U+0062 "b" */
    0x88, 0xe9, 0x9e,

    /* U+0063 "c" */
    0x69, 0x89, 0x60,

    /* U+0064 "d" */
    0x11, 0x79, 0x97,

    /* U+0065 "e" */
    0x69, 0xf8, 0x70,

    /* U+0066 "f" */
    0x34, 0xe4, 0x44,

    /* U+0067 "g" */
    0x79, 0x97, 0x16,

    /* U+0068 "h" */
    0x88, 0xe9, 0x99,

    /* U+0069 "i" */
    0xb8,

    /* U+006A "j" */
    0x20, 0xda, 0x80,

    /* U+006B "k" */
    0x88, 0xbc, 0xa9,

    /* U+006C "l" */
    0xfc,

    /* U+006D "m" */
    0xf5, 0x6b, 0x50,

    /* U+006E "n" */
    0xe9, 0x99,

    /* U+006F "o" */
    0x69, 0x96,

    /* U+0070 "p" */
    0xe9, 0x9e, 0x88,

    /* U+0071 "q" */
    0x79, 0x97, 0x11,

    /* U+0072 "r" */
    0x8b, 0xc8, 0x80,

    /* U+0073 "s" */
    0x78, 0x61, 0xe0,

    /* U+0074 "t" */
    0x5d, 0x22,

    /* U+0075 "u" */
    0x99, 0x97,

    /* U+0076 "v" */
    0x99, 0xa4,

    /* U+0077 "w" */
    0xad, 0x6a, 0xe0,

    /* U+0078 "x" */
    0xb5, 0x5a,

    /* U+0079 "y" */
    0xb5, 0x9c,

    /* U+007A "z" */
    0xf2, 0x48, 0xf0,

    /* U+007B "{" */
    0x0,

    /* U+007C "|" */
    0x0,

    /* U+007D "}" */
    0x9b, 0xfd, 0x0,

    /* U+007E "~" */
    0x66, 0x60
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 64, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 34, .box_w = 1, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2, .adv_w = 66, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 3, .adv_w = 98, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 7, .adv_w = 98, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 11, .adv_w = 114, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 16, .adv_w = 114, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 21, .adv_w = 66, .box_w = 1, .box_h = 2, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 22, .adv_w = 50, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 24, .adv_w = 50, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 26, .adv_w = 98, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 29, .adv_w = 98, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 33, .adv_w = 34, .box_w = 1, .box_h = 2, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 34, .adv_w = 82, .box_w = 3, .box_h = 1, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 35, .adv_w = 34, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 36, .adv_w = 66, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 39, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 42, .adv_w = 50, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 44, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 47, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 50, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 53, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 56, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 59, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 62, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 65, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 68, .adv_w = 66, .box_w = 1, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 69, .adv_w = 66, .box_w = 1, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 70, .adv_w = 66, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 72, .adv_w = 82, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 74, .adv_w = 66, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 76, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 79, .adv_w = 114, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 84, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 88, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 92, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 96, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 100, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 104, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 108, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 112, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 116, .adv_w = 66, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 119, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 123, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 127, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 131, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 135, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 139, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 143, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 147, .adv_w = 114, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 152, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 156, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 160, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 164, .adv_w = 130, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 170, .adv_w = 130, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 176, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 180, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 183, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 187, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 191, .adv_w = 50, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 193, .adv_w = 98, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 197, .adv_w = 50, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 199, .adv_w = 98, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 201, .adv_w = 66, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 202, .adv_w = 64, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 203, .adv_w = 98, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 207, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 210, .adv_w = 82, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 213, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 216, .adv_w = 82, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 219, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 222, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 225, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 228, .adv_w = 34, .box_w = 1, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 229, .adv_w = 66, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 232, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 235, .adv_w = 34, .box_w = 1, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 236, .adv_w = 98, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 239, .adv_w = 82, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 241, .adv_w = 82, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 243, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 246, .adv_w = 82, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 249, .adv_w = 82, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 252, .adv_w = 82, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 255, .adv_w = 66, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 257, .adv_w = 82, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 259, .adv_w = 82, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 261, .adv_w = 98, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 264, .adv_w = 66, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 266, .adv_w = 66, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 268, .adv_w = 82, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 271, .adv_w = 64, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 272, .adv_w = 64, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 273, .adv_w = 66, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 276, .adv_w = 114, .box_w = 6, .box_h = 2, .ofs_x = 0, .ofs_y = 2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t chibit_mr = {
#else
lv_font_t chibit_mr = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 7,          /*The maximum line height required by the font*/
    .base_line = 1,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
    .fallback = NULL,
    .user_data = NULL
};



#endif /*#if CHIBIT_MR*/

