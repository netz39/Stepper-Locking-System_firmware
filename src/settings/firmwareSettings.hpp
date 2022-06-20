#pragma once

#include "parameter_manager/SettingsContainer.hpp"
#include "parameter_manager/SettingsIO.hpp"
#include "parameter_manager/SettingsUser.hpp"
#include <string_view>

using namespace settings;

namespace firmwareSettings
{
constexpr std::string_view MotorMaxCurrent = "motor maximum current in percentage";
constexpr std::string_view MotorMaxSpeed = "motor maximum speed in steps per second";
constexpr std::string_view MotorMaxAcc = "motor maximum accerlation in steps per second^2";

// TODO: separate it
constexpr std::string_view MotorOverheatCounter = "counter of motor has reached overtemperature";
constexpr std::string_view MotorWarningTempCounter =
    "counter of motor has reached warning temperature";

inline constexpr std::array EntryArray = {
    // min, default, max, name
    SettingsEntry{0, 80, 100, MotorMaxCurrent, VariableType::integerType},            //
    SettingsEntry{0, 1000, 300000, MotorMaxSpeed, VariableType::integerType},         //
    SettingsEntry{0, 2500, 750000, MotorMaxAcc, VariableType::integerType},           //
    SettingsEntry{0, 0, 16777216, MotorOverheatCounter, VariableType::integerType},   //
    SettingsEntry{0, 0, 16777216, MotorWarningTempCounter, VariableType::integerType} //
};

using Container = SettingsContainer<EntryArray.size(), EntryArray>;
using IO = SettingsIO<EntryArray.size(), EntryArray>;

} // namespace firmwareSettings