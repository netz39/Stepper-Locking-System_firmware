#pragma once
#include "parameter_manager/SettingsEntry.hpp"
#include <core/SafeAssert.h>
#include <tuple>

namespace settings
{

/**
 * Searching, setting, getting settings values.
 * Does compiletime validation of static settings content.
 * @tparam SettingsCount
 * @tparam entryArray
 */
template <size_t SettingsCount, const std::array<SettingsEntry, SettingsCount> &entryArray>
class SettingsContainer
{
public:
    SettingsContainer()
    {
        static_assert(!containsDuplicates());
        static_assert(allStaticEntriesValid());
        resetAllToDefault();
        // TODO hookup settings IO and wait until loaded
    };

    /**
     * Retrieves a settings value by name / index.
     * Name templated overload determines setting existence at compile time. Zero lookup cost.
     * String overload ASSERTS setting existence. String search on every lookup.
     * Index lookup ASSERTS index validity. Zero lookup cost.
     *
     * @tparam T preferred return type, consider using util's unit system
     */
    template <const std::string_view &name, typename T = SettingsValue_t>
    [[nodiscard]] T getValue() const
    {
        constexpr size_t Index = getIndex<name>();
        return getValue<T>(Index);
    }

    template <typename T = SettingsValue_t>
    [[nodiscard]] T getValue(std::string_view name) const
    {
        return getValue<T>(getIndex(name));
    }

    template <typename T = SettingsValue_t>
    [[nodiscard]] T getValue(size_t index) const
    {
        SafeAssert(index < SettingsCount);
        const auto value = containerArray[index];
        if constexpr (std::is_same<T, SettingsValue_t>::value)
        {
            return value;
        }
        else
        {
            return static_cast<T>(value);
        }
    }

    /**
     * Returns the setting's minimum / default / maximum value. Asserts index bounds!
     * Consider using in combination with getIndex().
     * @param index index of setting
     * @return
     */
    [[nodiscard]] SettingsValue_t getMinValue(size_t index)
    {
        SafeAssert(index < SettingsCount);
        return entryArray[index].minValue;
    }

    [[nodiscard]] SettingsValue_t getMaxValue(size_t index)
    {
        SafeAssert(index < SettingsCount);
        return entryArray[index].maxValue;
    }

    [[nodiscard]] SettingsValue_t getDefaultValue(size_t index)
    {
        SafeAssert(index < SettingsCount);
        return entryArray[index].defaultValue;
    }

    /**
     * Sets new value by name / index.
     * Name templated overload determines setting existence at compile time. Zero cost.
     * String overload ASSERTS setting existence. String search on every usage.
     * Index lookup ASSERTS index validity. Zero cost.
     *
     * @return true on success, false if min / max bounds are violated
     */
    template <const std::string_view &name>
    bool setValue(const SettingsValue_t newValue)
    {
        constexpr size_t Index = getIndex<name>();
        return setValue(Index, newValue);
    }

    bool setValue(std::string_view name, const SettingsValue_t newValue)
    {
        return setValue(getIndex(name), newValue);
    }

    bool setValue(size_t Index, const float newValue)
    {
        SafeAssert(Index < SettingsCount);
        const auto MaxValue = entryArray[Index].maxValue;
        const auto MinValue = entryArray[Index].minValue;

        if (newValue > MaxValue || newValue < MinValue)
        {
            return false;
        }
        containerArray[Index] = newValue;
        return true;
    }

    /**
     * Get the values type by name/index - only relevant for UAVCAN's param server.
     */
    template <const std::string_view &name>
    [[nodiscard]] VariableType getVariableType() const
    {
        constexpr size_t Index = getIndex<name>();
        return getVariableType(Index);
    }

    [[nodiscard]] VariableType getVariableType(std::string_view name) const
    {
        return getVariableType(getIndex(name));
    }

    [[nodiscard]] VariableType getVariableType(size_t index) const
    {
        SafeAssert(index < SettingsCount);
        return entryArray[index].variableType;
    }

    /**
     * Retrieves a setting index.
     * Name templated overload determines setting existence at compile time. Zero cost.
     * String overload ASSERTS setting existence. String search on every usage. Asserts setting's
     * existence!
     */
    [[nodiscard]] size_t getIndex(std::string_view name) const
    {
        const std::tuple<bool, size_t> ret = getIndex_Aux(name);
        SafeAssert(std::get<0>(ret));
        return std::get<1>(ret);
    }
    template <const std::string_view &name>
    [[nodiscard]] static constexpr size_t getIndex()
    {
        constexpr std::tuple<bool, size_t> ret = getIndex_Aux(name);
        static_assert(std::get<0>(ret));
        return std::get<1>(ret);
    }

    [[nodiscard]] constexpr size_t size() const
    {
        return SettingsCount;
    }

    void resetAllToDefault()
    {
        for (size_t i = 0; i < SettingsCount; ++i)
        {
            containerArray[i] = entryArray[i].defaultValue;
        }
    }

    [[nodiscard]] constexpr const std::array<SettingsEntry, SettingsCount> &getAllSettings() const
    {
        return entryArray;
    }

    [[nodiscard]] bool doesSettingExist(std::string_view name) const
    {
        const auto [exists, index] = getIndex_Aux(name);
        return exists;
    }

    bool operator==(const SettingsContainer<SettingsCount, entryArray> &other) const
    {
        return containerArray == other.containerArray;
    }

    bool operator!=(const SettingsContainer<SettingsCount, entryArray> &other) const
    {
        return !((*this) == other);
    }

private:
    std::array<SettingsValue_t, SettingsCount> containerArray;

    [[nodiscard]] static constexpr std::tuple<bool, size_t>
    getIndex_Aux(const std::string_view &name)
    {
        for (size_t i = 0; i < SettingsCount; i++)
        {
            if (entryArray[i].hasSameName(name))
            {
                return std::make_tuple(true, i);
            }
        }
        return std::make_tuple(false, 0);
    }

    [[nodiscard]] static constexpr bool containsDuplicates()
    {
        for (const auto &i : entryArray)
        {
            for (const auto &j : entryArray)
            {
                if (std::addressof(i) == std::addressof(j))
                {
                    continue;
                }
                if (i.hasSameName(j.name))
                {
                    return true;
                }
            }
        }
        return false;
    }

    [[nodiscard]] static constexpr bool allStaticEntriesValid()
    {
        for (const auto &e : entryArray)
        {
            if (!e.isValid())
            {
                return false;
            }
        }
        return true;
    }
};

} // namespace settings