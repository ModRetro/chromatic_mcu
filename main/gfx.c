#include "gfx.h"

#include "osd.h"

static lv_style_t Background;
static lv_obj_t *pDisplayObj;

#define BG_COLOR 0xFF00FF

typedef struct
{
    lv_obj_t *pScreen;
} TimerCtx_t;

static TimerCtx_t _Ctx;

static void anim_timer_cb(lv_timer_t *timer)
{
    TimerCtx_t *timer_ctx = (TimerCtx_t *) timer->user_data;
    lv_obj_t *pScreen = timer_ctx->pScreen;

    OSD_HandleInputs();
    OSD_Draw(pScreen);
}

void Gfx_Start(lv_obj_t *const pScreen)
{
    // Initialize a tile that act as pages
    pDisplayObj = lv_tileview_create(pScreen);
    lv_obj_align(pDisplayObj, LV_ALIGN_TOP_RIGHT, 0, 0);

    // Create style to manipulate objects characteristics implicitly
    lv_style_init(&Background);

    // Change background color
    lv_obj_add_style(pDisplayObj, &Background, 0);
    lv_style_set_bg_color(&Background, lv_color_hex(BG_COLOR));

    _Ctx.pScreen = pScreen;

    // Create timer for animation
    lv_timer_create(anim_timer_cb, 20, &_Ctx);
}
