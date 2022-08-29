cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0048 NEW)
project(Stepper-Locking-System_firmware_test LANGUAGES CXX C ASM)

find_package(PkgConfig)
pkg_search_module(GTEST REQUIRED gtest)
pkg_search_module(GMOCK REQUIRED gmock)

# -Wno-int-to-pointer-cast suppresses warnings from HAL code that wants to access registers
# this code isn't even used in the tests so nothing to worry
# -Wno-deprecated-declarations is necessary as std::uncaught_exception (used within gtest)
# is deprecated in c++17 and has to be upgraded by google to not throw these errors
add_compile_options(-Wno-deprecated-declarations -Wno-int-to-pointer-cast) 

add_executable(${PROJECT_NAME}
        test/src/AnimationsRangeTest.cxx
        test/src/main.cxx
        )

target_include_directories(${PROJECT_NAME} PRIVATE
        .
        src/util/include
        test/fake
        test/stub
        )

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${GTEST_LDFLAGS}
        ${GMOCK_LDFLAGS}
        gcov
        )

target_link_options(${PROJECT_NAME} PRIVATE --coverage)
target_compile_options(${PROJECT_NAME} PRIVATE ${GTEST_CFLAGS} ${GMOCK_CFLAGS} --coverage)

add_test(${PROJECT_NAME} ${PROJECT_NAME})