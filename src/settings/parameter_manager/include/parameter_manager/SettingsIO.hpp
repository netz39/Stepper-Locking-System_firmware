#pragma once

#include "parameter_manager/SettingsContainer.hpp"
#include <core/hash.hpp>
#include <i2c-drivers/24lcxx.hpp>

namespace settings
{

/**
 * Handles saving non-static settings content to eeprom
 *
 * @tparam SettingsCount
 * @tparam entryArray
 */
template <size_t SettingsCount, const std::array<SettingsEntry, SettingsCount> &entryArray>
class SettingsIO
{
public:
    SettingsIO(Eeprom24LC64 &eeprom, SettingsContainer<SettingsCount, entryArray> &settings)
        : eeprom(eeprom),    //
          settings(settings) //
    {
    }
    virtual ~SettingsIO() = default;

    /**
     * Loads settings from EEPROM. Blocking. Updates SettingsContainer with read values on success.
     * Discards EEPROM content and writes defaults on failure.
     * @return true on success, false otherwise
     */
    virtual bool loadSettings()
    {
        eeprom.read(MemoryOffset, reinterpret_cast<uint8_t *>(&rawContent), sizeof(EepromContent));

        // verify header
        bool isValid =
            (rawContent.magicString == Signature) &&             //
            rawContent.settingsNamesHash == settingsNamesHash && //
            rawContent.settingsValuesHash == hashSettingsValues(rawContent.settingsContainer);

        // invalid, write sensible defaults
        if (!isValid)
        {
            settings.resetAllToDefault();
            saveSettings();
            return false;
        }

        // copy temporary settings to persistent instance
        bool saveRequired = false;
        for (const auto &settingEntry : settings.getAllSettings())
        {
            if (!settings.setValue(settingEntry.name,
                                   rawContent.settingsContainer.getValue(settingEntry.name)))
            {
                // read settings value is out of range, reset to default
                settings.setValue(settingEntry.name, settingEntry.defaultValue);
                rawContent.settingsContainer.setValue(settingEntry.name, settingEntry.defaultValue);
                saveRequired = true;
            }
        }
        if (saveRequired)
        {
            saveSettings();
        }
        return true;
    }

    /**
     * Writes settings to EEPROM. Blocking
     */
    virtual void saveSettings()
    {
        rawContent.magicString = Signature;
        rawContent.settingsNamesHash = settingsNamesHash;

        // copy persistent settings to temporary instance
        for (const auto &settingEntry : settings.getAllSettings())
        {
            const auto Value = settings.getValue(settingEntry.name);
            SafeAssert(rawContent.settingsContainer.setValue(settingEntry.name, Value));
        }
        rawContent.settingsValuesHash = hashSettingsValues(rawContent.settingsContainer);

        eeprom.write(MemoryOffset, reinterpret_cast<uint8_t *>(&rawContent), sizeof(EepromContent));
    }

    static constexpr size_t Signature = 0x0110CA6F;
    static constexpr uint16_t MemoryOffset = 0;
    struct EepromContent
    {
        // corruption unit test requires every member to be packed until the last one
        // but putting packed for the whole struct generates a warning
        // as optimisations to SettingsContainer may change its size
        // interfering with packing
        __attribute__((packed)) uint64_t settingsNamesHash = 0;
        __attribute__((packed)) uint64_t settingsValuesHash = 0;
        __attribute__((packed)) size_t magicString = Signature;
        SettingsContainer<SettingsCount, entryArray> settingsContainer;

        bool operator==(const EepromContent& other) const {
            return settingsNamesHash == other.settingsNamesHash &&
                   settingsValuesHash == other.settingsValuesHash &&
                   magicString == other.magicString &&
                   settingsContainer == other.settingsContainer;
        }
        bool operator!=(const EepromContent& other) const {
            return !((*this) == other);
        }
    };

    [[nodiscard]] static uint64_t
    hashSettingsValues(SettingsContainer<SettingsCount, entryArray> &container)
    {
        uint64_t hash = core::hash::HASH_SEED;
        for (const auto &settingEntry : container.getAllSettings())
        {
            SettingsValue_t value = container.getValue(settingEntry.name);
            const auto ptr = reinterpret_cast<const uint8_t *>(&value);
            hash = core::hash::fnvWithSeed(hash, ptr, ptr + sizeof(value));
        }
        return hash;
    }

private:
    Eeprom24LC64 &eeprom;
    SettingsContainer<SettingsCount, entryArray> &settings;
    EepromContent rawContent;

    [[nodiscard]] static uint64_t hashSettingsNames()
    {
        uint64_t hash = core::hash::HASH_SEED;
        for (const auto &settingEntry : entryArray)
        {
            hash = core::hash::fnvWithSeed(
                hash, reinterpret_cast<const uint8_t *>(std::begin(settingEntry.name)),
                reinterpret_cast<const uint8_t *>(std::end(settingEntry.name)));
        }
        return hash;
    }

    const uint64_t settingsNamesHash = hashSettingsNames();
};

} // namespace settings