#pragma once

#include "lvgl.h"
#include <stdint.h>

typedef enum LineConst {
    kLineConst_PointStartIdx,
    kLineConst_PointEndIdx,
    kLineConst_NumPoints,
} LineConst_t;

typedef struct Line {
    lv_obj_t* pObj;
    lv_point_t points[kLineConst_NumPoints]; // Start and end coordinates
    uint8_t width;
    uint32_t color;
} Line_t;
