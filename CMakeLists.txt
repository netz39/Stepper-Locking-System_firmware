cmake_minimum_required(VERSION 3.22)
set(CMAKE_PROJECT_NAME Stepper-Locking-System_firmware)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(BUILD_TESTING OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Add custom compiler flags, will also add flags to C++ compiler
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter")

# Include toolchain file
include(cubemx/cmake/gcc-arm-none-eabi.cmake)

# Disable warnings for rwx segments, as they are normal for embedded systems
add_link_options(-Wl,--no-warn-rwx-segments)

set(core_BUILDCONFIGURATION "embedded")
add_subdirectory(core)

# Enable CMake support for languages
enable_language(C CXX ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cubemx/cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# neccessary for TeensyStep
add_compile_definitions(STM32G0xx)

add_subdirectory(src/util)
add_subdirectory(src/eeprom-driver)
add_subdirectory(src/settings/settings-manager)

# Add sources to executable
target_sources(
    ${CMAKE_PROJECT_NAME} PRIVATE    
    src/Application.cxx

    src/analog_to_digital/AnalogDigital.cxx
    src/hall_encoder/AS5600.cxx
    src/hall_encoder/HallEncoder.cxx
    src/LED/AddressableLedDriver.cxx
    src/LED/LightController.cxx
    src/motor_control/MotorController.cxx
    src/motor_control/TMC2209.cxx
    src/motor_control/UartAccessor.cxx
    src/settings/Settings.cxx
    src/state_machine/StateMachine.cxx
    src/tactile_switches/TactileSwitches.cxx

    src/TeensyStep/src/ErrorHandler.cpp
    src/TeensyStep/src/Stepper.cpp
    src/TeensyStep/src/hal_port/Port.cpp
    src/TeensyStep/src/hal_port/HardwareTimer.cpp
    src/TeensyStep/src/hal_port/timer.c
    src/TeensyStep/src/timer/stm32/TimerField.cpp
    src/TeensyStep/src/timer/generic/TickTimer.cpp
)

# Add include paths
target_include_directories(
    ${CMAKE_PROJECT_NAME} PRIVATE
    src

    src/TeensyStep/src/
    src/TeensyStep/src/timer/
    src/TeensyStep/src/hal_port/
    src/TeensyStep/src/timer/generic/
    src/TeensyStep/src/timer/stm32/
)

# Add linked libraries
target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    stm32cubemx
    core
    util
    eeprom-driver
    settings-manager
)