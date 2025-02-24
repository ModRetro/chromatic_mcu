cmake_minimum_required(VERSION 3.22)


idf_component_register(SRCS
    "icon_bat_large_g.c"
    "icon_bat_large_y.c"
    "icon_bat_large_r.c"
    "icon_charging.c"
    "img_a_right.c"
    "img_b_left.c"
    "img_dot_white.c"
    "img_dot_grey.c"
    "img_arrow_up.c"
    "img_arrow_down.c"
    "img_brightness.c"
    "img_chromatic.c"
    "img_option_en.c"
    "img_option_dis.c"
    "img_toggle_off.c"
    "img_toggle_on.c"
    "menu_controls.c"
    "menu_display.c"
    "menu_status.c"
    "menu_system.c"
    REQUIRES lvgl
)
