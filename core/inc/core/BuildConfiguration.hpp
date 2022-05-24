#pragma once
#include "core/BuildConfigurationTypes.h"

namespace core
{
/**
 * @brief Provides constexpr bools for usage in a bit more macro-free code
 * Using the bools and if constexpr () is a more modern way of splitting
 * code paths between the different builds.
 * Macros are provided via compiler -D command.
 * Multiple definitions are caught at compile time.
 *
 */
struct BuildConfiguration
{
#if IS_EMBEDDED_BUILD()
    static constexpr bool IsEmbeddedBuild = true;
#else
    static constexpr bool IsEmbeddedBuild = false;
#endif

#if IS_TESTING_BUILD()
    static constexpr bool IsTestingBuild = true;
#else
    static constexpr bool IsTestingBuild = false;
#endif

#if IS_FUZZING_BUILD()
    static constexpr bool IsFuzzingBuild = true;
#else
    static constexpr bool IsFuzzingBuild = false;
#endif

#if IS_EMULATOR_BUILD()
    static constexpr bool IsEmulatorBuild = true;
#else
    static constexpr bool IsEmulatorBuild = false;
#endif

#if defined(BUILDCONFIG_DEBUG)
    static constexpr bool IsDebugBuild = true;
#else
    static constexpr bool IsDebugBuild = false;
#endif

#if defined(BUILDCONFIG_RELEASE)
    static constexpr bool IsReleaseBuild = true;
#else
    static constexpr bool IsReleaseBuild = false;
#endif

    static constexpr void checkBuild()
    {
        // sanity checks for macros
        static_assert(((IsEmbeddedBuild ? 1 : 0) + (IsTestingBuild ? 1 : 0) +
                       (IsFuzzingBuild ? 1 : 0) + (IsEmulatorBuild ? 1 : 0)) == 1);
        static_assert(((IsDebugBuild ? 1 : 0) + (IsReleaseBuild ? 1 : 0)) == 1);
    }
};

} // namespace core