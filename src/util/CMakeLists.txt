cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0048 NEW)

project(util LANGUAGES CXX C ASM)

add_library(${PROJECT_NAME} STATIC
        src/Button.cxx
        src/RecursiveMutex.cpp
        src/Mutex.cpp
        src/ScopedLock.cpp
        src/Task.cpp
        src/BinarySemaphore.cpp
        src/IndependentWatchdog.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
        include)

target_link_libraries(${PROJECT_NAME} freertos core)

if(NOT DEFINED isEmbeddedCompiler)
        message(FATAL_ERROR "Required variable isEmbeddedCompiler is not in scope")
endif()

if(${CMAKE_TESTING_ENABLED})
        find_package(PkgConfig)
        pkg_search_module(GTEST REQUIRED gtest)
        pkg_search_module(GMOCK REQUIRED gmock)

        add_executable(${PROJECT_NAME}_test
                test/src/main.cpp
                test/src/Angle.cpp
                test/src/units.cpp
                test/src/MapValue.cpp
        )
        target_link_libraries(${PROJECT_NAME}_test PRIVATE
                ${GTEST_LDFLAGS}
                ${GMOCK_LDFLAGS}
                gcov
                core
        )
        target_include_directories(${PROJECT_NAME}_test PRIVATE
                include)
        target_link_options(${PROJECT_NAME}_test PRIVATE --coverage)
        target_compile_options(${PROJECT_NAME}_test PRIVATE ${GTEST_CFLAGS} ${GMOCK_CFLAGS} --coverage)

        add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)
endif()