#pragma once

#include "BuildConfiguration.hpp"
#include "core/Version.h" // generated
#include <cstdint>

namespace core
{
struct BuildInformationTestingValues
{
    static constexpr uint32_t CommitHashShort = 42424242;
    static constexpr bool IsDirty = true;
    static constexpr bool IsDebugBuild = false;
    static constexpr char BuildTime[] = "testing_value_time";
    static constexpr char CommitHashLong[] = "testing_value_commit";
    static constexpr char BranchName[] = "testing_value_brach";
};

class BuildInformationHelper
{
public:
    static constexpr uint32_t getCommitHashShort()
    {
        if constexpr (BuildConfiguration::isEmbeddedBuild)
        {
            return BUILD_INFO_COMMIT_SHORT;
        }
        if constexpr (BuildConfiguration::isTestingBuild || BuildConfiguration::isFuzzingBuild)
        {
            return BuildInformationTestingValues::CommitHashShort;
        }
    }

    static constexpr bool getIsDirty()
    {
        if constexpr (BuildConfiguration::isEmbeddedBuild)
        {
            return BUILD_INFO_IS_DIRTY > 0;
        }
        if constexpr (BuildConfiguration::isTestingBuild || BuildConfiguration::isFuzzingBuild)
        {
            return BuildInformationTestingValues::IsDirty;
        }
    }

    // string literals can't easily be used as template parameters
    // so secondary step is neccessary
    static constexpr const char MacroBuildTime[] = BUILD_INFO_TIME;
    static constexpr const char MacroCommitHashLong[] = BUILD_INFO_COMMIT_LONG;
    static constexpr const char MacroBranchName[] = BUILD_INFO_BRANCH;

    template <const char *MacroValue, const char *FixedValue>
    static constexpr const char *get()
    {
        if constexpr (BuildConfiguration::isEmbeddedBuild)
        {
            return MacroValue;
        }
        if constexpr (BuildConfiguration::isTestingBuild || BuildConfiguration::isFuzzingBuild)
        {
            return FixedValue;
        }
    }
};

class BuildInformation
{
public:
    static constexpr uint32_t CommitHashShort = BuildInformationHelper::getCommitHashShort();
    static constexpr bool IsDirty = BuildInformationHelper::getIsDirty();
    static constexpr bool IsDebugBuild = BuildConfiguration::isDebugBuild;
    static constexpr const char *BuildTime =
        BuildInformationHelper::get<BuildInformationHelper::MacroBuildTime,
                                    BuildInformationTestingValues::BuildTime>();
    static constexpr const char *CommitHashLong =
        BuildInformationHelper::get<BuildInformationHelper::MacroCommitHashLong,
                                    BuildInformationTestingValues::CommitHashLong>();
    static constexpr const char *BranchName =
        BuildInformationHelper::get<BuildInformationHelper::MacroBranchName,
                                    BuildInformationTestingValues::BranchName>();
};
} // namespace core
