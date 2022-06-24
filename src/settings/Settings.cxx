#include "Settings.hpp"
#include "firmwareSettings.hpp"
// #include "sync.hpp"

void Settings::taskMain()
{
    settingsIO.loadSettings();
    // sync::signal(sync::ConfigurationLoaded);
    SettingsUser::notifySettingsUpdate();

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        settingsIO.saveSettings();
    }
}
