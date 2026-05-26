cmake_minimum_required(VERSION 3.26)

# MSVC UTF-8 encoding
if(MSVC)
    add_compile_options(/utf-8)
endif()

# keyboard_monitor — keyboard module only
if(HIDTOOL_BUILD_WITH_KEYBOARD)
    add_subdirectory(keyboard_monitor)
    add_subdirectory(keyboard_simulate)
endif()

# mouse_monitor — mouse module only
if(HIDTOOL_BUILD_WITH_MOUSE)
    add_subdirectory(mouse_monitor)
    add_subdirectory(mouse_simulate)
endif()

# hid_monitor / hid_simulate — both modules required
if(HIDTOOL_BUILD_WITH_KEYBOARD AND HIDTOOL_BUILD_WITH_MOUSE)
    add_subdirectory(hid_monitor)
    add_subdirectory(hid_simulate)
endif()
