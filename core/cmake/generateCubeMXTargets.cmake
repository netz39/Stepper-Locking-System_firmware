include_guard(GLOBAL)
include(${CMAKE_CURRENT_LIST_DIR}/extractBuildinfoFromMakefile.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/detectCompilerType.cmake)

# Extracts C and H files from cubemx makefile and creates three targets
# freertos - realtime os complete with heap, and port
# hel_headers - all cubemx generated headers without source files
# hal - complete hal with sources and headers
function(GENERATE_CUBEMX_TARGETS halDirectory generateFreertosTarget)
    GET_CUBEMX_VARIABLES(${halDirectory})
    DETECT_COMPILER_TYPE()

    set(HalSources)
    set(FreertosSources)
    foreach (entry IN LISTS MakeExport_SOURCES)
        if (entry MATCHES "Middlewares/Third_Party/FreeRTOS/Source/")
            if (entry MATCHES "CMSIS_RTOS_V2")
                # cmsis_os2.c is part of hal, but hides inside freertos sources
                list(APPEND HalSources ${entry})
            else ()
                list(APPEND FreertosSources ${entry})

            endif ()
        else ()
            list(APPEND HalSources ${entry})
        endif ()
    endforeach ()

    set(HalIncludes)
    set(FreertosIncludes)
    foreach (entry IN LISTS MakeExport_INCLUDES)
        if (entry MATCHES "Middlewares/Third_Party/FreeRTOS/Source")
            if (entry MATCHES "CMSIS_RTOS_V2")
                # cmsis_os2.c is part of hal, but hides inside freertos sources
                list(APPEND HalIncludes ${entry})
            else ()
                list(APPEND FreertosIncludes ${entry})

            endif ()
        else ()
            list(APPEND HalIncludes ${entry})
        endif ()
    endforeach ()

    add_library(hal_headers INTERFACE)
    target_include_directories(hal_headers INTERFACE ${HalIncludes})
    target_compile_definitions(hal_headers INTERFACE ${MakeExport_DEFS})

    # in testing a custom freertos version is built
    # this must be excluded to avoid duplicate target issues
    if (${generateFreertosTarget})
        # freertos
        add_library(freertos STATIC
                ${FreertosSources})
        target_include_directories(freertos PUBLIC ${FreertosIncludes})
        target_link_libraries(freertos PUBLIC hal_headers core)
    endif ()

    # complete hal
    # following target doesn't compile on non embedded builds
    # explicitly exclude from "All" build triggered by CTest
    add_library(hal OBJECT
            ${HalSources}
            )
    target_link_libraries(hal PUBLIC hal_headers freertos)
    set_target_properties(hal PROPERTIES
            EXCLUDE_FROM_ALL TRUE
            EXCLUDE_FROM_DEFAULT_BUILD TRUE)
endfunction()