#include "parameter_manager/SettingsEntry.hpp"
#include <gtest/gtest.h>

using namespace settings;

class SettingsEntryTest : public ::testing::Test
{
protected:
    static constexpr std::string_view Name1 =  "testEntry";
    static constexpr std::string_view Name2 =  "otherEntry";

    SettingsEntryTest()
        : entry(0, 1, 2, Name1), //
          entry_otherValues_sameName(1, 2, 3, Name1), //
          entry_otherName_sameValues(0, 1, 2, Name2)
    {
        static_assert(Name1.compare(Name2) != 0);
    }

    SettingsEntry entry;
    SettingsEntry entry_otherValues_sameName;
    SettingsEntry entry_otherName_sameValues;
};

TEST_F(SettingsEntryTest, isValid)
{
    {
        SettingsEntry entry(1, 2, 3, Name1);
        EXPECT_TRUE(entry.isValid());
    }

    {
        // min > max
        SettingsEntry entry(10, 2, 3, Name1);
        EXPECT_FALSE(entry.isValid());
    }

    {
        // default > max
        SettingsEntry entry(1, 20, 3, Name1);
        EXPECT_FALSE(entry.isValid());
    }

    {
        // max < min
        SettingsEntry entry(-1, 0, -3, Name1);
        EXPECT_FALSE(entry.isValid());
    }

    {
        // max < default
        SettingsEntry entry(-100, 2, -3, Name1);
        EXPECT_FALSE(entry.isValid());
    }
}

TEST_F(SettingsEntryTest, SameName)
{
    EXPECT_TRUE(entry.hasSameName(entry_otherValues_sameName.name));
    EXPECT_FALSE(entry.hasSameName(entry_otherName_sameValues.name));
}
