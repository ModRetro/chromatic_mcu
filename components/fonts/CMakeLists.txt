cmake_minimum_required(VERSION 3.22)

idf_component_register(
    SRCS "chibit_mr.c" "fingfai.c" "jf_dot_k14.c"
    PRIV_REQUIRES lvgl
)
