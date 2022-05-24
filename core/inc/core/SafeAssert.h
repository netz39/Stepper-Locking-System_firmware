#pragma once
#include "BuildConfigurationTypes.h"
#include <stdbool.h> // NOLINT

/**
 * @brief Build configuration independant assert
 *
 */

/**
 * @brief Asserts the given condition, aborts programm if false
 * When compiled for embedded, disables interrupts and enters endless loop
 * locking up the scheduler until the watchdog resets the device.
 * Throws std::runtime_exception in testing and fuzzing builds
 *
 * May be used in C or C++ Code.
 * @param condition
 */

#ifdef __cplusplus
extern "C"
{
#endif

    void SafeAssert_Aux(bool condition, int line, const char *file);

#ifdef __cplusplus
}
#endif

#if !IS_EMBEDDED_BUILD()
#define SafeAssert(cond) SafeAssert_Aux(cond, __LINE__, __FILE__) // NOLINT
#else
// avoids compiling in a bunch of path strings for embedded
#define SafeAssert(cond) SafeAssert_Aux(cond, 0, "NOVAL") // NOLINT
#endif

#define UAVCAN_ASSERT(cond) SafeAssert(cond)