#include "parameter_manager/SettingsContainer.hpp"

namespace FirmwareSettings
{
constexpr std::string_view CarMass = "car mass";


constexpr std::array EntryArray = {
    settings::SettingsEntry{1, -10, 10.0, CarMass},          //
};

using Container = settings::SettingsContainer<EntryArray.size(), EntryArray>;
}


int main()
{
    FirmwareSettings::Container container;
    return 0;
}