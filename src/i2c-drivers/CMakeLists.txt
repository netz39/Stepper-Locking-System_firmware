cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0048 NEW)

project(i2c-drivers LANGUAGES CXX C ASM)


add_library(${PROJECT_NAME} STATIC
        src/rtos_accessor.cpp
        src/as5600.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC
        include
        )

target_link_libraries(${PROJECT_NAME} util freertos core)