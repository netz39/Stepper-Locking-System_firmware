include_guard(GLOBAL)
include(${CMAKE_CURRENT_LIST_DIR}/detectCompilerType.cmake)
DETECT_COMPILER_TYPE()
add_compile_options(-fdiagnostics-color)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

if (NOT DEFINED isEmbeddedCompiler)
    message(FATAL_ERROR "Required variable isEmbeddedCompiler is not in scope")
endif ()
if (isEmbeddedCompiler)
    # fixes compiler detection with arm-none-eabi-gcc as cmake tries to
    # build an executable but bare metal doesn't work like this
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif ()

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source build detected! Generate cmake in an extra folder to avoid a mess of files generated in your folder")
endif ()

if (${isEmbeddedCompiler})
    message("arm compiler detected")
    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_SYSTEM_PROCESSOR arm)

    # cmake and the programs orchestrating it are stupid sometimes and only detect the C compiler
    # using the same path, set CXX/objcopy/objdump paths explicitly
    # this of course breakes when you pull apart your compiler programs
    # but why would you do that?
    get_filename_component(CompilerPath "${CMAKE_C_COMPILER}" PATH)
    if (CompilerPath STREQUAL "")
        # some ci compilers dont include the full path to compiler
        # assume default names for everything
        set(CMAKE_CXX_COMPILER "arm-none-eabi-g++" CACHE INTERNAL "")
        set(CMAKE_OBJCOPY "arm-none-eabi-objcopy" CACHE INTERNAL "")
        set(CMAKE_OBJDUMP "arm-none-eabi-objdump" CACHE INTERNAL "")
    else()
        set(CMAKE_CXX_COMPILER ${CompilerPath}/arm-none-eabi-g++ CACHE INTERNAL "")
        set(CMAKE_OBJCOPY ${CompilerPath}/arm-none-eabi-objcopy CACHE INTERNAL "")
        set(CMAKE_OBJDUMP ${CompilerPath}/arm-none-eabi-objdump CACHE INTERNAL "")
    endif()

    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
else ()
    message("non-arm compiler detected")
endif ()