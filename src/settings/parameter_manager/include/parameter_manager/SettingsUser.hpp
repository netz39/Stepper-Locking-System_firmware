#pragma once

#include <core/SafeAssert.h>

#include <array>
#include <cstdint>

namespace settings
{

/// Inherit from this class to automatically subscribe to settings changes and get
/// onSettingsUpdate() called.
class SettingsUser
{
public:
    SettingsUser()
    {
        if (registeredInstancesCount == MaxInstances)
            SafeAssert(false); // maximum number of instances reached

        index = registeredInstancesCount++;
        registeredInstances[index] = this;
    }
    ~SettingsUser()
    {
        registeredInstances[index] = nullptr;
    }

    /// User implemented function that queries relevant settings and save their own copy.
    /// Don't forget to call at least once to receive settings.
    virtual void onSettingsUpdate() = 0;

    /// Notifies all registered instances that settings changed.
    static void notifySettingsUpdate()
    {
        for (uint8_t i = 0; i < registeredInstancesCount; ++i)
            registeredInstances[i]->onSettingsUpdate();
    }

private:
    uint8_t index;
    static constexpr uint8_t MaxInstances = 16;
    inline static std::array<SettingsUser *, MaxInstances> registeredInstances{nullptr};
    inline static uint8_t registeredInstancesCount{0};
};
} // namespace settings