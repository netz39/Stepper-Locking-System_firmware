#pragma once

#include "firmwareSettings.hpp"
#include "wrappers/Task.hpp"

using util::wrappers::TaskWithMemberFunctionBase;

/// Load settings from EEPROM, send signals and save new settings to it, if needed
class Settings : public TaskWithMemberFunctionBase
{
public:
    explicit Settings(firmwareSettings::IO &settingsIO)
        : TaskWithMemberFunctionBase("settingsTask", 512,
                                     osPriorityAboveNormal5), //
          settingsIO(settingsIO){};

protected:
    [[noreturn]] void taskMain() override;

private:
    firmwareSettings::IO &settingsIO;
};
