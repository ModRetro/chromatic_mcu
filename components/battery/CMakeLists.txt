cmake_minimum_required(VERSION 3.22)

idf_component_register(SRCS "battery.c"
                    INCLUDE_DIRS "."
                    REQUIRES common images lvgl mutex)
