cmake_minimum_required(VERSION 3.13)

# Import Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(robosub_bms_board_rp2040 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Add your executable(s)
add_executable(robosub_bms_board_rp2040
    main.cpp
)

# Include header files if you have any in "include/"
target_include_directories(robosub_bms_board_rp2040 PRIVATE include)

# Link Pico libraries
target_link_libraries(robosub_bms_board_rp2040
    pico_stdlib
)

# Create UF2
pico_add_extra_outputs(robosub_bms_board_rp2040)
