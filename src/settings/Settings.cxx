#include "Settings.hpp"
#include "firmwareSettings.hpp"
#include "helpers/freertos.hpp"
#include "sync.hpp"

[[noreturn]] void Settings::taskMain()
{
    settingsIO.loadSettings();

    // increment reset counter at every task start and it save it
    settingsContainer.addToValue<firmwareSettings::ResetCounter>(1);
    settingsIO.saveSettings();

    SettingsUser::notifySettingsUpdate();
    sync::signal(sync::ConfigurationLoaded);

    while (true)
    {
        // save counters every 3 hours
        // EEPROM has erase/write cycles by 1'000'000
        // 1'000'000 cycles  / 8 writings per day = > 342 years
        constexpr auto NumberOfHoursToWait = 3;
        vTaskDelay(toOsTicks(NumberOfHoursToWait * 60.0_min));

        settingsContainer.addToValue<firmwareSettings::UptimeHours>(NumberOfHoursToWait);
        settingsIO.saveSettings();
    }
}
