#pragma once

#include "settings-manager/SettingsContainer.hpp"
#include "settings-manager/SettingsIO.hpp"
#include "settings-manager/SettingsUser.hpp"

#include "eeprom-driver/Eeprom24lcxx.hpp"
#include <string_view>

using namespace settings;

namespace firmwareSettings
{
constexpr std::string_view MotorMaxCurrent = "motor maximum current in percentage";
constexpr std::string_view MotorMaxSpeed = "motor maximum speed in steps per second";
constexpr std::string_view MotorMaxAcc = "motor maximum accerlation in steps per second^2";
constexpr std::string_view CalibrationSpeed = "calibration speed in steps per second";
constexpr std::string_view CalibrationAcc = "calibration accerlation in steps per second^2";
constexpr std::string_view InvertRotationDirection = "invert rotation direction respectively";

// TODO: separate it
constexpr std::string_view MotorOverheatCounter = "counter of motor has reached overtemperature";
constexpr std::string_view MotorWarningTempCounter =
    "counter of motor has reached warning temperature";
constexpr std::string_view HallFailureCounter = "counter of hall encoder communication failures";
constexpr std::string_view TmcFailureCounter = "counter of TMC driver communication failures";

constexpr std::string_view OpenCommandCounter = "number of incomed open commands";
constexpr std::string_view CloseCommandCounter = "number of incomed close commands";
constexpr std::string_view CompleteOpeningCounter = "number of completed openings";
constexpr std::string_view CompleteClosingCounter = "number of completed closings";
constexpr std::string_view ExternalMotorMovementCounter = "number of unexcepted motor movements";
constexpr std::string_view CriticalStepLossCounter = "number of big step losses";
constexpr std::string_view CalibrationCommandCounter = "number of incomed calibration commands";
constexpr std::string_view CalibrationInverseCommandCounter =
    "number of incomed inverted calibration commands";
constexpr std::string_view AbortCalibrationCounter = "number of aborted calibrations";
constexpr std::string_view CompleteCalibrationCounter = "number of completed calibrations";
constexpr std::string_view ResetCounter = "number of mcu resets";
constexpr std::string_view UptimeHours = "uptime in hours";

inline constexpr std::array EntryArray = {
    // min, default, max, name
    SettingsEntry{0, 100, 100, MotorMaxCurrent, VariableType::integerType},      //
    SettingsEntry{0, 10000, 300000, MotorMaxSpeed, VariableType::integerType},   //
    SettingsEntry{0, 12500, 750000, MotorMaxAcc, VariableType::integerType},     //
    SettingsEntry{0, 2000, 300000, CalibrationSpeed, VariableType::integerType}, //
    SettingsEntry{0, 12500, 750000, CalibrationAcc, VariableType::integerType},  //
    SettingsEntry{false, InvertRotationDirection},                               //

    // 16777216 is the max possible intregal value in a float
    SettingsEntry{0, 0, 16777216, MotorOverheatCounter, VariableType::integerType},             //
    SettingsEntry{0, 0, 16777216, MotorWarningTempCounter, VariableType::integerType},          //
    SettingsEntry{0, 0, 16777216, HallFailureCounter, VariableType::integerType},               //
    SettingsEntry{0, 0, 16777216, TmcFailureCounter, VariableType::integerType},                //
    SettingsEntry{0, 0, 16777216, OpenCommandCounter, VariableType::integerType},               //
    SettingsEntry{0, 0, 16777216, CloseCommandCounter, VariableType::integerType},              //
    SettingsEntry{0, 0, 16777216, CompleteOpeningCounter, VariableType::integerType},           //
    SettingsEntry{0, 0, 16777216, CompleteClosingCounter, VariableType::integerType},           //
    SettingsEntry{0, 0, 16777216, ExternalMotorMovementCounter, VariableType::integerType},     //
    SettingsEntry{0, 0, 16777216, CriticalStepLossCounter, VariableType::integerType},          //
    SettingsEntry{0, 0, 16777216, CalibrationCommandCounter, VariableType::integerType},        //
    SettingsEntry{0, 0, 16777216, CalibrationInverseCommandCounter, VariableType::integerType}, //
    SettingsEntry{0, 0, 16777216, AbortCalibrationCounter, VariableType::integerType},          //
    SettingsEntry{0, 0, 16777216, CompleteCalibrationCounter, VariableType::integerType},       //
    SettingsEntry{0, 0, 16777216, ResetCounter, VariableType::integerType},                     //
    SettingsEntry{0, 0, 16777216, UptimeHours, VariableType::integerType},                      //
};

using Container = SettingsContainer<EntryArray.size(), EntryArray>;
using IO = SettingsIO<EntryArray.size(), EntryArray, Eeprom24LC64>;

} // namespace firmwareSettings