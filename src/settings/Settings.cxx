#include "Settings.hpp"
#include "firmwareSettings.hpp"
#include "sync.hpp"

[[noreturn]] void Settings::taskMain()
{
    settingsIO.loadSettings();
    SettingsUser::notifySettingsUpdate();
    sync::signal(sync::ConfigurationLoaded);

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        settingsIO.saveSettings();
    }
}
