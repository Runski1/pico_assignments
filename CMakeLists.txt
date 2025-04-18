# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.6)
# Set board type because we are building for PicoW
set(PICO_BOARD pico_w)

# Please let cmake write compile_commands.json for my LSP - Matias
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)


# NETWORK ------------------------> 
if(DEFINED ENV{SSID})
    set(NETWORK_SSID "$ENV{SSID}")
else()
    set(NETWORK_SSID "default_ssid")  # Fallback value
    message(WARNING "SSID is not set! Using default_ssid.")
endif()

# Read Password
if(DEFINED ENV{WIFI_PASS})
    set(NETWORK_PASSWORD "$ENV{WIFI_PASS}")
else()
    set(NETWORK_PASSWORD "default_password")
    message(WARNING "PASSWORD is not set! Using default_password.")
endif()
if(DEFINED ENV{MQTT_SERVER_IP})
    set(MQTT_BROKER_IP "$ENV{MQTT_SERVER_IP}")
else()
    set(MQTT_BROKER_IP "asdf")
    message(WARNING "Broker IP is not set! Using default_password.")
endif()
# Pass to compiler as preprocessor definitions
add_compile_definitions(
    NETWORK_SSID="${NETWORK_SSID}"
    NETWORK_PASSWORD="${NETWORK_PASSWORD}"
    SERVER_IP="${MQTT_BROKER_IP}"
)
# <----------------------------NETWORK

# Set name of project (as PROJECT_NAME) and C/C   standards
# DON'T USE WHITESPACE IN PROJECT_NAME
project(display_demo)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

#add_compile_definitions(MQTTCLIENT_QOS2=1)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
)
# file(GLOB MQTT_SOURCES "../../src/network/paho.mqtt.embedded-c/MQTTPacket/src/*.c")

# Tell CMake where to find the executable source file
# USE RELATIVE PATHS
add_executable(${PROJECT_NAME}
        src/display.cpp
        pico-modbus/modbus/nanomodbus.h
        pico-modbus/modbus/nanomodbus.c
        pico-modbus/uart/PicoUart.cpp
        pico-modbus/uart/PicoUart.h
        pico-modbus/uart/RingBuffer.cpp
        pico-modbus/uart/RingBuffer.h
        pico-modbus/mqtt/Countdown.cpp
        pico-modbus/mqtt/Countdown.h
        pico-modbus/modbus/ModbusRegister.cpp
        pico-modbus/modbus/ModbusRegister.h
        pico-modbus/modbus/ModbusClient.cpp
        pico-modbus/modbus/ModbusClient.h
        pico-modbus/display/framebuf.cpp
        pico-modbus/display/framebuf.h
        pico-modbus/display/mono_vlsb.cpp
        pico-modbus/display/mono_vlsb.h
        pico-modbus/display/ssd1306.cpp
        pico-modbus/display/ssd1306.h
        pico-modbus/display/mono_horiz.cpp
        pico-modbus/display/mono_horiz.h
        pico-modbus/display/fonts/gfxfont.h
        pico-modbus/display/fonts/FreeMono12pt7b.h
        pico-modbus/display/st7789nobuf.cpp
        pico-modbus/display/st7789nobuf.h
        pico-modbus/display/i2c_device.h
        pico-modbus/i2c/PicoI2CDevice.cpp
        pico-modbus/i2c/PicoI2CDevice.h
        pico-modbus/i2c/PicoI2CBus.cpp
        pico-modbus/i2c/PicoI2CBus.h
        pico-modbus/display/spi_device.h
        pico-modbus/spi/PicoSPIBus.cpp
        pico-modbus/spi/PicoSPIBus.h
        pico-modbus/spi/PicoSPIDevice.cpp
        pico-modbus/spi/PicoSPIDevice.h
        pico-modbus/display/rgb_palette.cpp
        pico-modbus/display/rgb_palette.h
)
set(PICO_STD_LIBCXX ON)
# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# this adds a post build command that tries to copy uf2 to Pico
#add_custom_command(TARGET ${PROJECT_NAME}
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.uf2 d: )

target_include_directories(${PROJECT_NAME} PRIVATE
        pico-modbus/modbus
        pico-modbus/mqtt
        pico-modbus/uart
        pico-modbus/i2c
        pico-modbus/spi
        pico-modbus/display
        pico-modbus/display/fonts
)

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
        pico_stdlib
        hardware_i2c
        hardware_spi
        hardware_gpio
        pico_stdio
        pico_stdlib
        hardware_gpio
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
