cmake_minimum_required(VERSION 3.22)

idf_component_register(SRCS
                        "tab_dots/tab_dots.c"
                        "tab_list/tab_list.c"
                        "tab_shared.c"
                    INCLUDE_DIRS
                        "."
                        "tab_list/"
                        "tab_dots/"
                    REQUIRES common dlist)
