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
        // save counters every 12 hours
        vTaskDelay(toOsTicks(12 * 60.0_min));

        settingsContainer.addToValue<firmwareSettings::UptimeHours>(12);
        settingsIO.saveSettings();
    }
}
