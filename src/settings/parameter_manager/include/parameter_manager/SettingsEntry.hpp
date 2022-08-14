#pragma once
#include <string_view>

namespace settings
{
using SettingsValue_t = float;

enum class VariableType
{
    integerType,
    realType,
    booleanType,
};

/// A single settings entry. Static content.
class SettingsEntry
{
public:
    const SettingsValue_t minValue;
    const SettingsValue_t defaultValue;
    const SettingsValue_t maxValue;
    const std::string_view name;
    const VariableType variableType;

    //----------------------------------------------------------------------------------------------
    constexpr SettingsEntry(const SettingsValue_t min, const SettingsValue_t defaultValue,
                            const SettingsValue_t max, std::string_view name,
                            const VariableType variableType = VariableType::realType)
        : minValue{min}, defaultValue{defaultValue}, maxValue{max}, name{name}, variableType{
                                                                                    variableType}
    {
    }

    constexpr SettingsEntry(const bool defaultBoolValue, std::string_view name)
        : minValue{0}, defaultValue{defaultBoolValue ? 1.0f : 0.0f}, maxValue{1}, name{name},
          variableType{VariableType::booleanType}
    {
    }

    [[nodiscard]] constexpr bool isValid() const
    {
        return !(minValue > maxValue || defaultValue > maxValue || defaultValue < minValue);
    }

    [[nodiscard]] constexpr bool hasSameName(const std::string_view &otherName) const
    {
        return name.compare(otherName) == 0;
    }
};
} // namespace settings