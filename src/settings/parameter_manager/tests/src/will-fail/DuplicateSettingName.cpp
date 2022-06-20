#include "parameter_manager/SettingsContainer.hpp"

namespace FirmwareSettings
{
constexpr std::string_view CarMass = "car mass";
constexpr std::string_view CarWheelRadius = "car wheel radius";
constexpr std::string_view MotorMagnetCount = "motor magnet count";

constexpr std::array EntryArray = {
    settings::SettingsEntry{0.001, 2.5, 10.0, CarMass},          //
    settings::SettingsEntry{0.001, 0.05, 10.0, CarWheelRadius},  //
    settings::SettingsEntry{2.0, 24.0, 100.0, MotorMagnetCount}, //
    settings::SettingsEntry{2.0, 24.0, 100.0, MotorMagnetCount} //
};

using Container = settings::SettingsContainer<EntryArray.size(), EntryArray>;
}


int main()
{
    FirmwareSettings::Container container;
    return 0;
}