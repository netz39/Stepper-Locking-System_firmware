#include "TestSettings.hpp"
#include "fake/i2c-drivers/Fake24LC64.hpp"
#include "stub/BusAccessor.hpp"
#include <array>
#include <core/hash.hpp>
#include <gtest/gtest.h>
#include <numeric>

using namespace settings;
using TestSettings::Container;
using TestSettings::IO;

class SettingsIOTest : public ::testing::Test
{
protected:
    SettingsIOTest() : eeprom(accessor), settingsIo(eeprom, settingsContainer)
    {
        // placement information of IO::EepromContent members
        loadEepromContentOffsets();
    }

    BusAccessorStub accessor;
    FakeEeprom24LC64 eeprom;
    Container settingsContainer;
    IO settingsIo;

    IO::EepromContent temporaryContent;

    using Offset_t = uint64_t;
    using OffsetEntry_t = std::pair<uint64_t, uint64_t>;
    void loadEepromContentOffsets();

    OffsetEntry_t NamesHashOffset;
    OffsetEntry_t ValuesHashOffset;
    OffsetEntry_t MagicStringOffset;
    OffsetEntry_t SettingsContainerOffset;
    // keep size in line with number of OffsetEntry_t above, too big array will fail asserts in
    // loadEepromContentOffsets
    std::array<OffsetEntry_t, 4> allOffsets;
};

void SettingsIOTest::loadEepromContentOffsets()
{
    // get offsets so we can precisely corrupt data
    static_assert(sizeof(OffsetEntry_t::first) == sizeof(OffsetEntry_t::second));
    static_assert(sizeof(OffsetEntry_t::first) == sizeof(Offset_t));
    static_assert(
        sizeof(IO::EepromContent *) <= sizeof(Offset_t),
        "your architecture has a higher pointer length, increase uint64_t everywhere here");

    NamesHashOffset = std::pair(reinterpret_cast<Offset_t>(&temporaryContent.settingsNamesHash) -
                                    reinterpret_cast<Offset_t>(&temporaryContent),
                                0);
    ValuesHashOffset = std::pair(reinterpret_cast<Offset_t>(&temporaryContent.settingsValuesHash) -
                                     reinterpret_cast<Offset_t>(&temporaryContent),
                                 0);
    MagicStringOffset = std::pair(reinterpret_cast<Offset_t>(&temporaryContent.magicString) -
                                      reinterpret_cast<Offset_t>(&temporaryContent),
                                  0);
    SettingsContainerOffset =
        std::pair(reinterpret_cast<Offset_t>(&temporaryContent.settingsContainer) -
                      reinterpret_cast<Offset_t>(&temporaryContent),
                  0);
    allOffsets = {NamesHashOffset, ValuesHashOffset, MagicStringOffset, SettingsContainerOffset};

    // not the same offsets, offsets sorted ascending
    Offset_t accumulatedSize = 0;
    for (int i = 0; i < allOffsets.size() - 1; ++i)
    {
        ASSERT_LT(allOffsets[i], allOffsets[i + 1]);
        allOffsets[0].second = allOffsets[i + 1].first - allOffsets[i].first;
        accumulatedSize += allOffsets[0].second;
    }

    // not outside of struct
    for (auto &trgt : allOffsets)
    {
        ASSERT_LT(trgt.first, sizeof(IO::EepromContent));
        ASSERT_LT(trgt.second, sizeof(IO::EepromContent));
    }
    allOffsets[allOffsets.size() - 1].second =
        sizeof(IO::EepromContent) - allOffsets[allOffsets.size() - 1].first;

    // every member of struct is targeted, assuming settingsContainer is last
    ASSERT_EQ(accumulatedSize + sizeof(IO::EepromContent::settingsContainer),
              sizeof(IO::EepromContent));
}

TEST_F(SettingsIOTest, initFromEmptyEeprom)
{
    // eeprom is filled with 0xFF fresh out of the package,
    // ensure a default settings initialisation and write back of
    // default values
    settingsIo.loadSettings();
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));
    ASSERT_EQ(temporaryContent.settingsContainer, settingsContainer);
}

TEST_F(SettingsIOTest, initFromDefaultEeprom)
{
    // init eeprom content, which fills it with default values and saves
    ASSERT_FALSE(settingsIo.loadSettings());
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));

    // change a setting, so we can spot if default values are read again later
    static_assert(TestSettings::Entry1_min != TestSettings::Entry1_default);
    settingsContainer.setValue(TestSettings::Entry1, TestSettings::Entry1_min);
    EXPECT_NE(temporaryContent.settingsContainer, settingsContainer);

    // restore default eeprom and check if we are back to default
    eeprom.write(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                 sizeof(IO::EepromContent));
    ASSERT_TRUE(settingsIo.loadSettings());
    EXPECT_EQ(temporaryContent.settingsContainer, settingsContainer);
}

TEST_F(SettingsIOTest, initFromCorruptedEeprom)
{
    // default values
    IO::EepromContent cleanDefault;
    ASSERT_FALSE(settingsIo.loadSettings());
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&cleanDefault),
                sizeof(IO::EepromContent));

    // corrupt every single byte of every single member and see
    // it content resets
    for (const auto &target : allOffsets)
    {
        for (uint64_t i = 0; i < target.second; ++i)
        {
            temporaryContent = cleanDefault;
            auto location = reinterpret_cast<uint8_t *>(&temporaryContent);
            location += target.first + i;
            (*location)++;

            // reset eeprom to clean and check if it reads ok
            eeprom.write(0, reinterpret_cast<uint8_t *>(&cleanDefault), sizeof(IO::EepromContent));
            ASSERT_TRUE(settingsIo.loadSettings());

            // now the corrupted stuff
            eeprom.write(0, reinterpret_cast<uint8_t *>(&temporaryContent),
                         sizeof(IO::EepromContent));
            ASSERT_FALSE(settingsIo.loadSettings());
        }
    }
}

TEST_F(SettingsIOTest, saveSettings)
{
    // init eeprom content, which fills it with default values and saves
    ASSERT_FALSE(settingsIo.loadSettings());
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));

    // change a setting
    static_assert(TestSettings::Entry1_min != TestSettings::Entry1_default);
    ASSERT_EQ(settingsContainer.getValue(TestSettings::Entry1), TestSettings::Entry1_default);
    settingsContainer.setValue(TestSettings::Entry1, TestSettings::Entry1_min);
    ASSERT_EQ(settingsContainer.getValue(TestSettings::Entry1), TestSettings::Entry1_min);
    EXPECT_NE(temporaryContent.settingsContainer, settingsContainer); // applying setting worked

    // assure altered content is saved
    IO::EepromContent alteredContent;
    settingsIo.saveSettings();
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&alteredContent),
                sizeof(IO::EepromContent));
    ASSERT_NE(temporaryContent, alteredContent);
}

TEST_F(SettingsIOTest, EepromContentEquality)
{
    ASSERT_EQ(temporaryContent,temporaryContent);
    auto other = temporaryContent;
    other.settingsContainer.setValue(TestSettings::Entry1, TestSettings::Entry1_min);
    ASSERT_NE(temporaryContent,other);
}

TEST_F(SettingsIOTest, BoundsCheckFailOnLoad)
{
    // init eeprom content, which fills it with default values and saves
    ASSERT_FALSE(settingsIo.loadSettings());
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));

    // bounds check fails when eeprom content is loaded after
    // min / max of one or more SettingsEntries was changed
    // so that the freshly loaded value doesn't fit anymore

    // instead of creating a changed duplicate of TestSettings
    // this test will directly modify a value in temporaryContent with aid of
    // the precalculated offsets from loadEepromContentOffsets
    // also the values hash will be recalculated, so we read without causing a
    // reset to defaults

    // change a setting outside of bounds
    auto rawData = reinterpret_cast<SettingsValue_t *>(&temporaryContent.settingsContainer);
    ASSERT_LT(temporaryContent.settingsContainer.getValue(TestSettings::Entry1),
              TestSettings::Entry1_max);
    rawData[0] = TestSettings::Entry1_max + static_cast<SettingsValue_t>(1);
    ASSERT_GT(temporaryContent.settingsContainer.getValue(TestSettings::Entry1),
              TestSettings::Entry1_max);
    temporaryContent.settingsValuesHash =
        IO::hashSettingsValues(temporaryContent.settingsContainer);

    // write back to eeprom
    eeprom.write(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                 sizeof(IO::EepromContent));

    // load without reset
    ASSERT_TRUE(settingsIo.loadSettings());

    // check if reset to default worked
    ASSERT_EQ(settingsContainer.getValue(TestSettings::Entry1),
              TestSettings::Entry1_default);
}