cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0048 NEW)
project(testapp LANGUAGES CXX C ASM)

find_package(PkgConfig)
pkg_search_module(GTEST REQUIRED gtest)
pkg_search_module(GMOCK REQUIRED gmock)

set(CMAKE_CXX_STANDARD 17)
# -Wno-int-to-pointer-cast suppresses warnings from HAL code that wants to access registers
# this code isn't even used in the tests so nothing to worry
# -Wno-deprecated-declarations is necessary as std::uncaught_exception (used within gtest)
# is deprecated in c++17 and has to be upgraded by google to not throw these errors
add_compile_options(-Wno-deprecated-declarations -Wno-int-to-pointer-cast)

include("libraries/core/cmake/generateCubeMXTargets.cmake")
set(core_BUILDCONFIGURATION "testing")
set(core_TRACEALYZER false)
add_subdirectory(libraries/core)

add_subdirectory(test/freertos_linux)

set(bus_node_base_UAVCAN_MemPool_Size 16384)
set(bus_node_base_UAVCAN_RX_Queue_Size 128)
set(bus_node_base_STM32_UAVCAN_Timer_Number 7)
set(bus_node_base_STM32_UAVCAN_PCLK1 42000000)
set(bus_node_base_STM32_UAVCAN_TIMCLK1 84000000)
add_subdirectory(libraries/bus-node-base)

add_subdirectory(libraries/iir1)
add_subdirectory(libraries/imu-driver)
add_subdirectory(libraries/i2c-drivers)
add_subdirectory(libraries/util)
GENERATE_CUBEMX_TARGETS("drive_controller.cubemx" false)

add_subdirectory(libraries/parameter_manager)

add_executable(${PROJECT_NAME}
        test/src/main.cpp
        test/src/mytest.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE
        test/inc
        src)

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${GTEST_LDFLAGS}
        ${GMOCK_LDFLAGS}
        hal_headers
        core
        iir1
        imu-driver
        bus_node_base_linux
        i2c-drivers
        util
        parameter_manager
        )
target_compile_options(${PROJECT_NAME} PRIVATE ${GTEST_CFLAGS} ${GMOCK_CFLAGS})

include(CTest)
add_test(drive_controller ${PROJECT_NAME})