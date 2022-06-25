#include <gtest/gtest.h>
#include <numbers>
#include <units/si/angle.hpp>

using namespace units::si;

TEST(units_angle, scalar)
{
    EXPECT_FLOAT_EQ((90.0_deg).getMagnitude(), Angle(std::numbers::pi / 2).getMagnitude());
    EXPECT_FLOAT_EQ((90.0_deg).getMagnitude(), std::numbers::pi / 2.0);
    EXPECT_FLOAT_EQ((90.0_deg).getMagnitude(scale::rad),
                    Angle(std::numbers::pi / 2.0).getMagnitude(scale::rad));
}

TEST(units_angle, scale)
{
    Angle myAngle;
    myAngle.setMagnitude(90.0f, scale::deg);

    EXPECT_FLOAT_EQ(Angle(std::numbers::pi / 2).getMagnitude(), myAngle.getMagnitude());
}