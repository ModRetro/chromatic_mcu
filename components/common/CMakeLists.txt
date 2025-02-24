cmake_minimum_required(VERSION 3.22)

idf_component_register(
    SRCS "osd_shared.c"
    INCLUDE_DIRS "."
    REQUIRES button dlist lvgl fonts
)
