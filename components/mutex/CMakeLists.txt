cmake_minimum_required(VERSION 3.22)

idf_component_register(SRCS "mutex.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES freertos)
