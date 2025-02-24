cmake_minimum_required(VERSION 3.22)

idf_component_register(SRCS "menu_mgr.c"
                    INCLUDE_DIRS "."
                    REQUIRES common dlist lvgl osd tab)
