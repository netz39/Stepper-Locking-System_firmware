include_guard(GLOBAL)
include(${CMAKE_CURRENT_LIST_DIR}/extractBuildinfoFromMakefile.cmake)

# Invokes GET_CUBEMX_VARIABLES to retrieve defines and options from cubemx makefile
# assumes halDirectory is given as relative path seen from folder of top level script
# (linker needs full path to .ld file, so correct path assembly is critical)

function(SETUP_BUILD halDirectory firmwareName Cstandard CXXstandard)
    GET_CUBEMX_VARIABLES(${halDirectory})

    if (NOT DEFINED MakeExport_SOURCES OR NOT DEFINED MakeExport_MCU_Flags OR NOT DEFINED MakeExport_DEFS OR NOT DEFINED MakeExport_INCLUDES
            OR NOT DEFINED MakeExport_LDSCRIPT)
        message(FATAL_ERROR "Required variables are not in scope")
    endif ()

    set(Optimisaiton)
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        message("SETUP_BUILD configuring for Release.")
        set(Optimisation -O2 -g)
    elseif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        message("SETUP_BUILD configuring for Debug.")
        set(Optimisation -Og -g)
    else ()
        message(FATAL_ERROR "Unknown Buildtype")
    endif ()

    set(Specs --specs=nano.specs --specs=nosys.specs)
    set(CFlags -std=${Cstandard} ${Optimisation} -fno-builtin-log)
    set(CppFlags -ffunction-sections -fdata-sections -fno-common
            -pedantic -Wall -Wextra
            -Wno-unused-parameter -Wno-unused-variable
            -fexec-charset=cp1252
            ${Specs})
    set(CxxFlags -std=${CXXstandard} ${Optimisation}
            -fno-exceptions -fno-rtti -fno-unwind-tables -Wno-register -fno-math-errno)

    # generator expressions have some quirky behaviour when multiple lists separated by spaces
    # are directly used, to avoid it just concat all lists beforehand
    set(c_lang_options ${MakeExport_MCU_Flags} ${CFlags} ${CppFlags})
    set(cpp_lang_options ${MakeExport_MCU_Flags} ${CxxFlags} ${CppFlags})
    set(asm_lang_options -x assembler-with-cpp ${MakeExport_MCU_Flags} ${CFlags} ${CppFlags})

    # must come before any target has been defined or else options won't stick
    # all targets must be built with the same compile options or bad things happen
    add_compile_options(
            "$<$<COMPILE_LANGUAGE:C>:${c_lang_options}>"
            "$<$<COMPILE_LANGUAGE:CXX>:${cpp_lang_options}>"
            "$<$<COMPILE_LANGUAGE:ASM>:${asm_lang_options}>"
    )
    add_definitions(${MakeExport_DEFS})

    # -u _printf_float removes a bunch of dependencies on various
    # internal libc functions. Adding nosys specs would achieve the same thing
    add_link_options(${Specs}
            --static
            -Wl,--gc-sections
            -Wl,--print-memory-usage
            -Wl,-Map=${firmwareName}.map
            -u _printf_float
            -Wl,--start-group -lc_nano -lgcc -lnosys -Wl,--end-group
            -T${CMAKE_CURRENT_SOURCE_DIR}/${MakeExport_LDSCRIPT}
            ${MakeExport_MCU_Flags})
endfunction()
