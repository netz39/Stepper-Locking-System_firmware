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
constexpr std::string_view CalibrationSpeed = "calibration speed in steps per second";
constexpr std::string_view CalibrationAcc = "calibration accerlation in steps per second^2";

// TODO: separate it
constexpr std::string_view MotorOverheatCounter = "counter of motor has reached overtemperature";
constexpr std::string_view MotorWarningTempCounter =
    "counter of motor has reached warning temperature";

inline constexpr std::array EntryArray = {
    // min, default, max, name
    SettingsEntry{0, 100, 100, MotorMaxCurrent, VariableType::integerType},     //
    SettingsEntry{0, 35000, 50000, MotorMaxSpeed, VariableType::integerType},   //
    SettingsEntry{0, 7000, 10000, MotorMaxAcc, VariableType::integerType},      //
    SettingsEntry{0, 2000, 50000, CalibrationSpeed, VariableType::integerType}, //
    SettingsEntry{0, 7000, 10000, CalibrationAcc, VariableType::integerType},   //

    SettingsEntry{0, 0, 16777216, MotorOverheatCounter, VariableType::integerType},   //
    SettingsEntry{0, 0, 16777216, MotorWarningTempCounter, VariableType::integerType} //
};

using Container = SettingsContainer<EntryArray.size(), EntryArray>;
using IO = SettingsIO<EntryArray.size(), EntryArray>;

} // namespace firmwareSettings