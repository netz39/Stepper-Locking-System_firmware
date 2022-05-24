include_guard(GLOBAL)

# Sets isEmbeddedCompiler true or false in parent scope
function(DETECT_COMPILER_TYPE)
    if (${CMAKE_C_COMPILER} MATCHES "arm-none-eabi-")
        set(isEmbeddedCompiler true PARENT_SCOPE)
    else()
        set(isEmbeddedCompiler false PARENT_SCOPE)
    endif()
endfunction()