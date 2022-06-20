#include "parameter_manager/SettingsIO.hpp"
#include <gmock/gmock.h>

template <size_t SettingsCount,
          const std::array<settings::SettingsEntry, SettingsCount> &entryArray>
class SettingsIOMock : public settings::SettingsIO<SettingsCount, entryArray>
{
public:
    SettingsIOMock(Eeprom24LC64 &eeprom, settings::SettingsContainer<SettingsCount, entryArray> &settings)
        : settings::SettingsIO<SettingsCount, entryArray>(eeprom, settings)
    {
    };
    MOCK_METHOD(bool, loadSettings, (), (override));
    MOCK_METHOD(void, saveSettings, (), (override));
};