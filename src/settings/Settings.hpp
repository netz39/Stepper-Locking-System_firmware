#pragma once

#include "firmwareSettings.hpp"
#include "wrappers/Task.hpp"

using util::wrappers::TaskWithMemberFunctionBase;

/// Load settings from EEPROM, send signals and save new settings to it, if needed
class Settings : public TaskWithMemberFunctionBase
{
public:
    Settings(firmwareSettings::IO &settingsIO, firmwareSettings::Container &settingsContainer)
        : TaskWithMemberFunctionBase("settingsTask", 512,
                                     osPriorityAboveNormal5), //
          settingsIO(settingsIO),                             //
          settingsContainer(settingsContainer)                //
          {};

protected:
    [[noreturn]] void taskMain() override;

private:
    firmwareSettings::IO &settingsIO;
    firmwareSettings::Container &settingsContainer;

    void setValues();
};
