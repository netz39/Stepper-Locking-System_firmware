cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0048 NEW)
project(Stepper-Locking-System_firmware LANGUAGES CXX C ASM)

# boilerplate cmake for setting up compiler flags, extraction from cubemx code
# internal macros and such
include("core/cmake/setupEmbeddedBuild.cmake")
include("core/cmake/setupEmbeddedExtraFiles.cmake")
include("core/cmake/generateCubeMXTargets.cmake")

set(HalDir "cubemx")

# sets up compiler flags, you may change the c++ and c version here
SETUP_BUILD(${HalDir} ${PROJECT_NAME} c11 c++17)

# generates cmake libraries for hal and freertos
GENERATE_CUBEMX_TARGETS(${HalDir} true)

# instructs core to set all internal macros for embedded configuration
# Affects if arm specific code is compiled in which would SEGFAULT on non arm targets
set(core_BUILDCONFIGURATION "embedded")

add_subdirectory(core)
add_subdirectory(src/util)
add_subdirectory(src/LED/gcem)
add_subdirectory(src/eeprom-driver)
add_subdirectory(src/settings/settings-manager)

add_compile_definitions(STM32G0xx)

set(INCLUDE_FILES
        src

        src/TeensyStep/src/
        src/TeensyStep/src/timer/
        src/TeensyStep/src/hal_port/
        src/TeensyStep/src/timer/generic/
        src/TeensyStep/src/timer/stm32/
)

set(Sources
        src/Application.cxx
        src/sync.cxx

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

# Instructs CMake to build your firmware
add_executable(${PROJECT_NAME} ${Sources})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_FILES})

# Every call to add_subdirectory() will load the libraries' CMakeLists.txt file. Those will contain one or more
# static / object libraries whose names you have to add here. target_link_libraries will not only see that the
# libraries' code is added to your firmware but also sets up include directories. So you don't have to add them manually.
# Read libraries' README files for information about specifics.
target_link_libraries(${PROJECT_NAME} PRIVATE

        # Core's libraries
        hal
        core
        freertos
        util
        gcem
        eeprom-driver
        settings-manager
)

# Sets up generation of .list and .bin files after your firmware has generated.
# Makes firmware image generate as an .elf file.
# Also performs checks for some common bugs that can happen in buildsystem configuration
SETUP_EXTRA_FILE_GENERATION(${PROJECT_NAME} ../core)
