cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0048 NEW)

#TODO change in your libraries' name
project(util LANGUAGES CXX C ASM)

#TODO add library files
#TODO if your library only contains headers you need to change STATIC to INTERFACE
#TODO and all subsequent target_.... function need to use INTERFACE instead of PUBLIC / PRIVATE / ...
add_library(${PROJECT_NAME} STATIC
        src/mySrc)

#TODO add include directories that will be later exposed
#TODO to the program using your library
target_include_directories(${PROJECT_NAME} PUBLIC
        include
        )

# TODO add private includes
#target_include_directories(${PROJECT_NAME} PRIVATE
#        include
#        )

# TODO add your dependencies. If you are using freertos internally you probably want 'freertos'
# TODO for hal functionality only include 'hal_headers' as using hal directly is reserved for the firmware executable
# TODO there is no drawback for using hal_headers! Read up on the buildsystems design for more information about this.
# TODO when including hal headers make sure you use core/chip/chip.h as this will include the right headers
# TODO for f1 / f4 / ...
# target_link_libraries(${PROJECT_NAME} freertos core)


#
# Testing
# All libraries assume that at some point you've included core/cmake/compilerSetup.cmake but for avoiding
# Configuration errors please leave the NOT DEFINED if check
#
if (NOT DEFINED isEmbeddedCompiler)
    message(FATAL_ERROR "Required variable isEmbeddedCompiler is not in scope")
endif ()
if (NOT ${isEmbeddedCompiler})
    # Sets up google test, google mock
    # Ubuntu packages are: libgtest-dev pkg-config libgmock-dev
    find_package(PkgConfig)
    pkg_search_module(GTEST REQUIRED gtest)
    pkg_search_module(GMOCK REQUIRED gmock)
    set(CMAKE_CXX_STANDARD 17)

    #TODO add your test application files
    #TODO check the core/template/testMain.cpp file
    add_executable(${PROJECT_NAME}_test
            test/src/testMain.cpp
            )

    # Google test / mock setup
    target_link_libraries(${PROJECT_NAME}_test PRIVATE
            ${GTEST_LDFLAGS}
            ${GMOCK_LDFLAGS}
            gcov)
    target_link_options(${PROJECT_NAME}_test PUBLIC --coverage)
    target_compile_options(${PROJECT_NAME}_test PUBLIC ${GTEST_CFLAGS} ${GMOCK_CFLAGS} --coverage)

    # registers your test application with CTest. A tool for easy management of
    # Test suites provided by the various libraries
    include(CTest)
    add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)
endif ()