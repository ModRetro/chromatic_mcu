cmake_minimum_required(VERSION 3.5)

add_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mcu_fw)
