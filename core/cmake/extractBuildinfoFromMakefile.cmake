include_guard(GLOBAL)
# call specially created make file that prints out variables
# expecting output from the makefile code in mk/CubeMXMakefileInstrumentation.mk

# Sets the following variables in parent scope
# MakeExport_SOURCES, MakeExport_MCU_Flags, MakeExport_DEFS, MakeExport_INCLUDES, MakeExport_LDSCRIPT
# prefixDirectory will be applied to sources, includes and ldscript
function(GET_CUBEMX_VARIABLES prefixDirectory)
    execute_process(COMMAND make --no-print-directory -C ${PROJECT_SOURCE_DIR}/${prefixDirectory}/
            OUTPUT_VARIABLE out)

    # split output into list
    # a list is a string separated by ; for cmake btw
    string(REPLACE "\n" ";" out "${out}")
    list(LENGTH out length)
    if (NOT length MATCHES 6)
        # 5 entries + one empty line
        message(FATAL_ERROR "Unable to extract information from makefile: list of lists is too short. Did you properly prepare/instrument it?")
    endif ()

    set(foundSources false)
    set(foundMCUFlags false)
    set(foundDefs false)
    set(foundIncludes false)
    set(foundLDScript false)
    foreach (entry IN LISTS out)
        # strip away any -I or -D
        string(REPLACE "-I" "" entry "${entry}")
        #string(REPLACE "-D" "" entry "${entry}")

        # split into list, entries are expected to be whitespace separated
        string(REPLACE " " ";" entryList "${entry}")

        list(LENGTH entryList length)
        if (${length} LESS 2)
            continue()
        endif ()

        # get first element (name of entry), save it and remove it from the main list
        list(GET entryList 0 entryName)
        list(REMOVE_AT entryList 0)

        if (entryName MATCHES "^MakeExport_SOURCES")
            list(TRANSFORM entryList PREPEND "${prefixDirectory}/")
            set(MakeExport_SOURCES ${entryList} PARENT_SCOPE)
            set(foundSources true)
            continue()
        endif ()
        if (entryName MATCHES "^MakeExport_MCU_Flags")
            set(MakeExport_MCU_Flags ${entryList} PARENT_SCOPE)
            set(foundMCUFlags true)
            continue()
        endif ()
        if (entryName MATCHES "^MakeExport_DEFS")
            set(MakeExport_DEFS ${entryList} PARENT_SCOPE)
            set(foundDefs true)
            continue()
        endif ()
        if (entryName MATCHES "^MakeExport_INCLUDES")
            list(TRANSFORM entryList PREPEND "${prefixDirectory}/")
            set(MakeExport_INCLUDES ${entryList} PARENT_SCOPE)
            set(foundIncludes true)
            continue()
        endif ()
        if (entryName MATCHES "^MakeExport_LDSCRIPT")
            list(TRANSFORM entryList PREPEND "${prefixDirectory}/")
            set(MakeExport_LDSCRIPT ${entryList} PARENT_SCOPE)
            set(foundLDScript true)
            continue()
        endif ()
    endforeach ()
    if (NOT foundLDScript OR NOT foundIncludes OR NOT foundDefs OR NOT foundMCUFlags OR NOT foundSources)
        message(FATAL_ERROR "CubeMX is not properly prepared with export statement")
    endif ()
endfunction()