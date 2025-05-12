cmake_minimum_required(VERSION 3.22)

idf_component_register(SRCS "button.c"
                    INCLUDE_DIRS "."
                    REQUIRES common mutex osd console)
