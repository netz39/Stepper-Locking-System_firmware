#include "TestSettings.hpp"
#include <exception>
#include <gtest/gtest.h>

using namespace settings;
using namespace TestSettings;

class SettingsContainerTest : public ::testing::Test
{
protected:
    SettingsContainerTest()
    {
    }
    TestSettings::Container settingsContainer;
};

TEST_F(SettingsContainerTest, Size)
{
    EXPECT_EQ(settingsContainer.size(), TestSettings::EntryArray.size());
}

TEST_F(SettingsContainerTest, initWithDefaultValues)
{
    static_assert(Entry1_default != Entry2_default);
    static_assert(Entry2_default != Entry3_default);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry1), Entry1_default);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry2), Entry2_default);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry3), Entry3_default);
}

TEST_F(SettingsContainerTest, indexBoundsChecks)
{
    // [[nondiscard]] warning fixes
    SettingsValue_t a;
    EXPECT_THROW(a = settingsContainer.getValue(TestSettings::EntryArray.size() + 1),
                 std::runtime_error);
    EXPECT_THROW(a = settingsContainer.getMinValue(TestSettings::EntryArray.size() + 1),
                 std::runtime_error);
    EXPECT_THROW(a = settingsContainer.getMaxValue(TestSettings::EntryArray.size() + 1),
                 std::runtime_error);
    EXPECT_THROW(a = settingsContainer.getDefaultValue(TestSettings::EntryArray.size() + 1),
                 std::runtime_error);
    EXPECT_THROW(settingsContainer.setValue(TestSettings::EntryArray.size() + 1, 0),
                 std::runtime_error);
}

TEST_F(SettingsContainerTest, existanceChecks)
{
    // [[nondiscard]] warning fixes
    SettingsValue_t a;
    size_t index;
    static constexpr std::string_view UnknownName = "[[[unknownName]]]";
    static_assert(UnknownName != Entry1);
    static_assert(UnknownName != Entry2);
    static_assert(UnknownName != Entry3);
    static_assert(UnknownName != EntryBoolean);
    static_assert(UnknownName != EntryInteger);
    static_assert(TestSettings::EntryArray.size() == 5);

    EXPECT_THROW(a = settingsContainer.getValue(UnknownName), std::runtime_error);
    EXPECT_THROW(settingsContainer.setValue(UnknownName, 0), std::runtime_error);
    EXPECT_THROW(index = settingsContainer.getIndex(UnknownName), std::runtime_error);
    EXPECT_FALSE(settingsContainer.doesSettingExist(UnknownName));
    EXPECT_TRUE(settingsContainer.doesSettingExist(Entry1));
    EXPECT_TRUE(settingsContainer.doesSettingExist(Entry2));
    EXPECT_TRUE(settingsContainer.doesSettingExist(Entry3));
    EXPECT_TRUE(settingsContainer.doesSettingExist(EntryBoolean));
    EXPECT_TRUE(settingsContainer.doesSettingExist(EntryInteger));
}

TEST_F(SettingsContainerTest, setValues)
{
    // relying on default initialisation
    EXPECT_TRUE(settingsContainer.setValue(Entry1, Entry1_max));
    EXPECT_TRUE(settingsContainer.setValue(Entry2, Entry2_max));
    EXPECT_TRUE(settingsContainer.setValue(Entry3, Entry3_max));
    EXPECT_TRUE(settingsContainer.setValue(EntryInteger, EntryInteger_max));

    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry1), Entry1_max);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry2), Entry2_max);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry3), Entry3_max);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(EntryInteger), EntryInteger_max);

    EXPECT_TRUE(settingsContainer.setValue(Entry1, Entry1_min));
    EXPECT_TRUE(settingsContainer.setValue(Entry2, Entry2_min));
    EXPECT_TRUE(settingsContainer.setValue(Entry3, Entry3_min));
    EXPECT_TRUE(settingsContainer.setValue(EntryInteger, EntryInteger_min));

    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry1), Entry1_min);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry2), Entry2_min);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry3), Entry3_min);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(EntryInteger), EntryInteger_min);

    EXPECT_TRUE(settingsContainer.setValue<Entry1>(Entry1_default));
    EXPECT_TRUE(settingsContainer.setValue<Entry2>(Entry2_default));
    EXPECT_TRUE(settingsContainer.setValue<Entry3>(Entry3_default));
    EXPECT_TRUE(settingsContainer.setValue<EntryInteger>(EntryInteger_default));

    EXPECT_FLOAT_EQ(settingsContainer.getValue<Entry1>(), Entry1_default);
    EXPECT_FLOAT_EQ(settingsContainer.getValue<Entry2>(), Entry2_default);
    EXPECT_FLOAT_EQ(settingsContainer.getValue<Entry3>(), Entry3_default);
    EXPECT_FLOAT_EQ(settingsContainer.getValue<EntryInteger>(), EntryInteger_default);

    EXPECT_TRUE(settingsContainer.setValue(EntryBoolean, false));
    EXPECT_FLOAT_EQ(settingsContainer.getValue(EntryBoolean), false);

    EXPECT_TRUE(settingsContainer.setValue(EntryBoolean, true));
    EXPECT_FLOAT_EQ(settingsContainer.getValue(EntryBoolean), true);

    static constexpr auto entry1Index = Container::getIndex<Entry1>();
    EXPECT_FALSE(settingsContainer.setValue(Entry1, Entry1_min - 1));
    EXPECT_FALSE(settingsContainer.setValue(Entry1, Entry1_max + 1));
    EXPECT_FALSE(settingsContainer.setValue(entry1Index, Entry1_min - 1));
    EXPECT_FALSE(settingsContainer.setValue(entry1Index, Entry1_max + 1));
}

TEST_F(SettingsContainerTest, setGetValuesByIndex)
{
    static constexpr auto entry1Index = Container::getIndex<Entry1>();
    static constexpr auto entry2Index = Container::getIndex<Entry2>();
    static constexpr auto entry3Index = Container::getIndex<Entry3>();

    EXPECT_EQ(entry1Index, settingsContainer.getIndex(Entry1));
    EXPECT_EQ(entry2Index, settingsContainer.getIndex(Entry2));
    EXPECT_EQ(entry3Index, settingsContainer.getIndex(Entry3));

    EXPECT_TRUE(settingsContainer.setValue(entry1Index, Entry1_max));
    EXPECT_TRUE(settingsContainer.setValue(entry2Index, Entry2_max));
    EXPECT_TRUE(settingsContainer.setValue(entry3Index, Entry3_max));

    EXPECT_FLOAT_EQ(settingsContainer.getValue(entry1Index), Entry1_max);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(entry2Index), Entry2_max);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(entry3Index), Entry3_max);

    EXPECT_TRUE(settingsContainer.setValue(entry1Index, Entry1_min));
    EXPECT_TRUE(settingsContainer.setValue(entry2Index, Entry2_min));
    EXPECT_TRUE(settingsContainer.setValue(entry3Index, Entry3_min));

    EXPECT_FLOAT_EQ(settingsContainer.getValue(entry1Index), Entry1_min);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(entry2Index), Entry2_min);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(entry3Index), Entry3_min);
}

TEST_F(SettingsContainerTest, resetAllToDefault)
{
    static_assert(Entry1_max != Entry2_max);
    static_assert(Entry2_max != Entry3_max);

    static_assert(Entry1_default != Entry2_default);
    static_assert(Entry2_default != Entry3_default);

    static_assert(Entry1_max != Entry1_default);
    static_assert(Entry2_max != Entry2_default);
    static_assert(Entry3_max != Entry3_default);

    EXPECT_TRUE(settingsContainer.setValue(Entry1, Entry1_max));
    EXPECT_TRUE(settingsContainer.setValue(Entry2, Entry2_max));
    EXPECT_TRUE(settingsContainer.setValue(Entry3, Entry3_max));

    settingsContainer.resetAllToDefault();

    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry1), Entry1_default);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry2), Entry2_default);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry3), Entry3_default);
}

TEST_F(SettingsContainerTest, getAllSettings)
{
    ASSERT_EQ(std::addressof(EntryArray), std::addressof(settingsContainer.getAllSettings()));
}

TEST_F(SettingsContainerTest, getMin_Default_MaxValue)
{
    static constexpr auto entry1Index = Container::getIndex<Entry1>();
    static constexpr auto entry2Index = Container::getIndex<Entry2>();
    static constexpr auto entry3Index = Container::getIndex<Entry3>();

    static_assert(Entry1_min != Entry2_min);
    static_assert(Entry2_min != Entry3_min);
    ASSERT_EQ(settingsContainer.getMinValue(entry1Index), Entry1_min);
    ASSERT_EQ(settingsContainer.getMinValue(entry2Index), Entry2_min);
    ASSERT_EQ(settingsContainer.getMinValue(entry3Index), Entry3_min);

    static_assert(Entry1_default != Entry2_default);
    static_assert(Entry2_default != Entry3_default);
    ASSERT_EQ(settingsContainer.getDefaultValue(entry1Index), Entry1_default);
    ASSERT_EQ(settingsContainer.getDefaultValue(entry2Index), Entry2_default);
    ASSERT_EQ(settingsContainer.getDefaultValue(entry3Index), Entry3_default);

    static_assert(Entry1_max != Entry2_max);
    static_assert(Entry2_max != Entry3_max);
    ASSERT_EQ(settingsContainer.getMaxValue(entry1Index), Entry1_max);
    ASSERT_EQ(settingsContainer.getMaxValue(entry2Index), Entry2_max);
    ASSERT_EQ(settingsContainer.getMaxValue(entry3Index), Entry3_max);
}