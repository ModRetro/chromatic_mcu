cmake_minimum_required(VERSION 3.22)

idf_component_register(SRCS
                        "controls/dpad_ctl.c"
                        "controls/hotkeys.c"
                        "palette/style.c"
                        "display/frameblend.c"
                        "display/color_correct_lcd.c"
                        "display/color_correct_usb.c"
                        "display/low_batt_icon_ctl.c"
                        "display/screen_transit_ctl.c"
                        "osd.c"
                        "osd_default.c"
                        "status/brightness.c"
                        "status/fw.c"
                        "system/silent.c"
                        "system/serial_num.c"
                        "system/player_num.c"
                    INCLUDE_DIRS
                        "."
                        "controls/"
                        "status/"
                        "palette/"
                        "display/"
                        "system/"
                    REQUIRES battery common dlist menu_mgr
                    PRIV_REQUIRES esp_app_format settings console efuse)
