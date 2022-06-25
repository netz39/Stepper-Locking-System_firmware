# parameter_manager

Container for settings with easy UAVCAN access, EEPROM saving and compile time checks for static content.

### CMake setup

Expects *isEmbeddedCompiler* variable from *core/cmake/detectCompilerType.cmake* in parent scope.

Requires following libraries:

- core
- i2c-drivers
- bus_node_base

Exports following libraries:

- parameter_manager

----
### Overview / Terminology

A setting has four static (compiled in) and one dynamic property.

static:
- Name
- Minimum value
- Default value
- Maximum value

dynamic:
- Current value

EEPROM will only save dynamic content but will hash the string names to detect static settings changes. Min / Max
changes may cause settings to be reset to default when their newly loaded value is out of bounds. The internal
type representation of Current value is always float. But you can choose the type (real, integer, boolean),
which should be presented in UAVCAN's parameter server. So you can change in GUI tool for instance values
as integer values instead of point numbers, even if it is internally saved as float.

----
### How to

You start by creating a header file for your static content (name, min, max, default). Everything MUST be constexpr here
for compile-time checks to work. Requires at least C++17. Compile-time checks include duplicate names and Min <=
Default <= Max sanity checking.

```cpp
#pragma once
#include "parameter_manager/ParameterManager.hpp"

namespace FirmwareSettings
{
constexpr std::string_view CarMass = "car mass";
constexpr std::string_view CarWheelRadius = "car wheel radius";
constexpr std::string_view MotorMagnetCount = "motor magnet count";
constexpr std::string_view DeviceAddress = "adress of device (integer)";
constexpr std::string_view IsServoEnabled = "servo enable (boolean)";

// declare our EntryArray with inline keyword is needed to get rid of linker errors and compiler warnings
// otherwise C++ re-declaring it for every time it is included
// which will cause undefined behaviour, linker errors and compiler warnings

inline constexpr std::array EntryArray = {
    // Min, Default, Max, Name
    settings::SettingsEntry{0.001, 2.5, 10.0, CarMass},           //
    settings::SettingsEntry{0.001, 0.05, 10.0, CarWheelRadius},   //
    settings::SettingsEntry{2.0, 24.0 , 100.0, MotorMagnetCount}, //
    settings::SettingsEntry{0, 0x42 , 0xFFFF, DeviceAddress, settings::VariableType::integerType}, //
    settings::SettingsEntry{true, IsServoEnabled}, //
};
using Container = settings::SettingsContainer<EntryArray.size(), EntryArray>;
using IO = settings::SettingsIO<EntryArray.size(), EntryArray>;
using ParameterManager = settings::ParameterManager<EntryArray.size(), EntryArray>;
}
```

Now we instantiate the settings classes in a .cpp/.cxx file:

```cpp
#include "parameter_manager/ParameterManager.hpp"
#include <i2c-drivers/24lcxx.hpp>

Eeprom24LC64 eeprom (/* your eeprom i2c settings */);

FirmwareSettings::Container settingsContainer;
FirmwareSettings::IO settingsIO(eeprom, settingsContainer);
FirmwareSettings::ParameterManager parameterManager(settingsContainer, settingsIO);
```

*SettingsContainer* allows you to lookup settings. *SettingsIO* handles writing settings to EEPROM.
*ParameterManager* integrates settings with UAVCAN's *GetSet* configuration system.
**Be aware that setting's values can change in runtime!** see section down below for efficient working with settings.

----
### Settings Lookup

You are free to the normal *settingsContainer.getValue(SettingName)* function but be aware that this will search through
a list of strings for every lookup. When you need the settings values often you must use a more efficient approach. *
SettingsContainer* allows you to look up a setting at compile time:

```cpp
// slow to execute
// this will do a bunch of string search every time you want to retrieve the setting
float myVal = settingsContainer.getValue(FirmwareSettings::CarMass);

// way quicker
float myVal2 = settingsContainer.getValue<FirmwareSettings::CarMass>();

// same is also true for the setValue functions
```

If you are asking yourself why going through all this trouble is worth it, consider the alternative with hard-coded
indices. Any small change in the settings order will cause nasty bugs where wrong values are delivered to your code.
This approach will make it much harder to make such mistakes.

All classes using settings should inherit from here and implement the `onSettingsUpdate()` function. This function is guaranteed to be called at least once when the EEPROM is finished initializing (your class must be contstructed before that of course). When called before EEPROM is ready you will only get default values.
Will be called when someone updates the value by calling the static `notifySettingsUpdate()` function.

Here is an example .hpp file related to example in **How To**:

```cpp
#pragma once

#include "settings/SettingsContainer.hpp"
#include "settings/SettingsUser.hpp"
#include <units/si/mass.hpp>
#include <units/si/length.hpp>

class Motor : public settings::SettingsUser
{
public:
    Motor(settings::SettingsContainer &settings): settings(settings);

    void onSettingsUpdate() override
    {
        carMass = settings.getValue<units::si::Mass>(settings::CarMass);
        carWheelRadius = settings.getValue<units::si::Length>(settings::CarWheelRadius);
        motorMagnetCount = settings.getValue(settings::MotorMagnetCount);
    }

private:
    settings::SettingsContainer &settings;

    units::si::Mass carMass{0.0_kg};
    units::si:Length carWheelRadius
    float motorMagnetCount
}
```