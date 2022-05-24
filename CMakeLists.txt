# CMake splitter decides if firmware or test application is built depending on
# how the compiler is named. arm-none-eabi compiler will build firmware. Everything else tests.
# This is necessary as cmake is not built to easily invoke different compilers for different
# Build-targets.

cmake_minimum_required(VERSION 3.15)
include("core/cmake/compilerSetup.cmake")

project(cmake_splitter VERSION 0.0.1 LANGUAGES CXX C)

if (NOT DEFINED isEmbeddedCompiler)
    message(FATAL_ERROR "compiler type not known")
endif()

if(${isEmbeddedCompiler})
    include("src/CMakeLists.txt")
else()
    include("test/CMakeLists.txt")
endif()
