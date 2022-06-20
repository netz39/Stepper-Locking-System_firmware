#pragma once

#include "firmwareSettings.hpp"
#include "wrappers/Task.hpp"

using util::wrappers::TaskWithMemberFunctionBase;

class Settings : public TaskWithMemberFunctionBase
{
public:
    Settings(firmwareSettings::IO &settingsIO)
        : TaskWithMemberFunctionBase("settingsTask", 512,
                                     osPriorityAboveNormal5), //
          settingsIO(settingsIO){};

protected:
    void taskMain() override;

private:
    firmwareSettings::IO &settingsIO;
};
