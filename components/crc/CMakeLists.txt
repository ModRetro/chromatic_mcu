cmake_minimum_required(VERSION 3.22)

idf_component_register(SRCS "crc8_sae_j1850.c"
                    INCLUDE_DIRS "."
                    REQUIRES common)
