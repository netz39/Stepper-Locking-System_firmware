#include "parameter_manager/SettingsUser.hpp"
#include "TestSettings.hpp"
#include "fake/i2c-drivers/Fake24LC64.hpp"
#include "parameter_manager/SettingsIO.hpp"
#include "stub/BusAccessor.hpp"
#include <gtest/gtest.h>

namespace
{
using namespace settings;

class SettingsIOTest
{
public:
    SettingsIOTest() : eeprom(accessor), settingsIo(eeprom, settingsContainer)
    {
    }

    BusAccessorStub accessor;
    FakeEeprom24LC64 eeprom;
    TestSettings::Container settingsContainer;
    TestSettings::IO settingsIo;
};

class SettingsUserTest : public settings::SettingsUser, public ::testing::Test
{
protected:
    SettingsUserTest()
    {
        settingsIoTest.settingsIo.loadSettings();
    }
    void onSettingsUpdate() override;

    bool settingsUpdateFunctionCalled = false;
    SettingsIOTest settingsIoTest;

    static constexpr std::string_view EntryName = TestSettings::Entry2;
    float value = 0.0f;
};

void SettingsUserTest::onSettingsUpdate()
{
    settingsUpdateFunctionCalled = true;
    value = settingsIoTest.settingsContainer.getValue<EntryName>();
}

TEST_F(SettingsUserTest, notifySettings)
{
    EXPECT_FALSE(settingsUpdateFunctionCalled);
    EXPECT_FLOAT_EQ(value, 0.0f);

    SettingsUser::notifySettingsUpdate();

    EXPECT_TRUE(settingsUpdateFunctionCalled);
    EXPECT_FLOAT_EQ(value, 20.0f);
}
} // namespace