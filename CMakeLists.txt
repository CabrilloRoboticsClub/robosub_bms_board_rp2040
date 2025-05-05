cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(robosub_bms_board_rp2040 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()


add_executable(robosub_bms_board_rp2040
    main.c
    src/gpio_control.c
    src/bme280_wrapper.c
    src/ina780.c
    lib/bme280/bme280.c
    lib/pico-ssd1306/ssd1306.c
    # add more excutables
)

# Include any headers in the current directory (like gpio_control.h)
target_include_directories(robosub_bms_board_rp2040 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/lib/bme280
    ${CMAKE_CURRENT_LIST_DIR}/lib/pico-ssd1306
    ${CMAKE_CURRENT_LIST_DIR}/lib/pico-ssd1306
)

# Link with the Pico standard library
target_link_libraries(robosub_bms_board_rp2040
    pico_stdlib
    hardware_i2c
)

# Configure Pico
pico_enable_stdio_usb(robosub_bms_board_rp2040 1)
pico_enable_stdio_uart(robosub_bms_board_rp2040 0)

# Create the UF2
pico_add_extra_outputs(robosub_bms_board_rp2040)
