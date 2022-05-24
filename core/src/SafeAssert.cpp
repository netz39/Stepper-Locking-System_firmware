#include "core/SafeAssert.h"
#include "core/BuildConfiguration.hpp"

#if IS_EMBEDDED_BUILD()
#include <FreeRTOS.h>
#include <task.h>
void SafeAssert_Aux(bool condition, int line, const char *file)
{
    if (!condition)
    {
        taskDISABLE_INTERRUPTS();
        if constexpr (core::BuildConfiguration::IsDebugBuild)
        {
            __asm("bkpt");
        }
        for (;;)
        {
        }
    }
}
#else
#include <sstream>
#include <stdexcept>

void SafeAssert_Aux(bool condition, int line, const char *file)
{
    if (!condition)
    {
        std::stringstream ss;
        ss << "Assertion Failed " << file << ":" << line;
        throw std::runtime_error(ss.str());
    }
}
#endif