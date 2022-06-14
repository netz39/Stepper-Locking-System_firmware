#include <gtest/gtest.h>
#include <units/si/scalar.hpp>
#include <numbers>

using namespace units::si;
using namespace units;

TEST(Value, constructors)
{
    {
        Value<Scalar> v;
        EXPECT_FLOAT_EQ(v.getMagnitude(), 0.0f);
    }
    {
        constexpr auto Target = 10.0f;
        Value<Scalar> v(Target);
        EXPECT_FLOAT_EQ(v.getMagnitude(), Target);
    }
    {
        constexpr auto Val = 10.0f;
        constexpr auto Off = 5.0f;
        Offset<Scalar> o(Off);
        Value<Scalar> v(Val, o);
        EXPECT_FLOAT_EQ(v.getMagnitude(), Val - Off);
    }
    {
        constexpr auto Val = 10.0f;
        constexpr auto Scal = 2.0f;
        Scale<Scalar> s(Scal);
        Value<Scalar> v(Val, s);
        EXPECT_FLOAT_EQ(v.getMagnitude(), Val / Scal);
    }
}

TEST(Value, getMagnitude)
{
    constexpr auto Target = 10.0f;
    constexpr auto Off = 5.0f;
    constexpr auto Scal = 2.0f;
    Value<Scalar> v(Target);
    Offset<Scalar> o(Off);
    Scale<Scalar> s(Scal);

    EXPECT_FLOAT_EQ(v.getMagnitude(), Target);
    EXPECT_FLOAT_EQ(v.getMagnitude(o), Target + Off);
    EXPECT_FLOAT_EQ(v.getMagnitude(s), Target * Scal);
}

TEST(Value, setMagnitude)
{
    constexpr auto Target = 10.0f;
    constexpr auto Off = 5.0f;
    constexpr auto Scal = 5.0f;
    Offset<Scalar> o(Off);
    Scale<Scalar> s(Scal);

    {
        Value<Scalar> v;
        v.setMagnitude(Target);
        EXPECT_FLOAT_EQ(v.getMagnitude(), Target);
    }
    {
        Value<Scalar> v;
        v.setMagnitude(Target, o);
        EXPECT_FLOAT_EQ(v.getMagnitude(), Target - Off);
    }
    {
        Value<Scalar> v;
        v.setMagnitude(Target, s);
        EXPECT_FLOAT_EQ(v.getMagnitude(), Target / Scal);
    }
}

TEST(Value, abs)
{
    constexpr auto Target = 10.0f;
    {
        Value<Scalar> v(Target);
        EXPECT_FLOAT_EQ(v.abs(), Target);
    }
    {
        Value<Scalar> v(-Target);
        EXPECT_FLOAT_EQ(v.abs(), Target);
    }
}


TEST(units, abs)
{
    EXPECT_FLOAT_EQ(units::abs(1.0_).getMagnitude(), units::abs(-1.0_).getMagnitude());
    EXPECT_FLOAT_EQ(units::abs(1.0_).getMagnitude(), units::abs(1.0_).getMagnitude());
}

TEST(units, min)
{
    static constexpr auto Small = 1.0_;
    static constexpr auto Negative = -1.0_;
    static constexpr auto Big = 10.0_;

    EXPECT_FLOAT_EQ(units::min(Small, Big).getMagnitude(), Small.getMagnitude());
    EXPECT_FLOAT_EQ(units::min(Big, Small).getMagnitude(), Small.getMagnitude());
    EXPECT_FLOAT_EQ(units::min(Small, Negative).getMagnitude(), Negative.getMagnitude());
    EXPECT_FLOAT_EQ(units::min(Negative, Small).getMagnitude(), Negative.getMagnitude());
    EXPECT_FLOAT_EQ(units::min(Negative, Big).getMagnitude(), Negative.getMagnitude());
}

TEST(units, max)
{
    static constexpr auto Small = 1.0_;
    static constexpr auto Negative = -1.0_;
    static constexpr auto Big = 10.0_;

    EXPECT_FLOAT_EQ(units::max(Small, Big).getMagnitude(), Big.getMagnitude());
    EXPECT_FLOAT_EQ(units::max(Big, Small).getMagnitude(), Big.getMagnitude());
    EXPECT_FLOAT_EQ(units::max(Small, Negative).getMagnitude(), Small.getMagnitude());
    EXPECT_FLOAT_EQ(units::max(Negative, Small).getMagnitude(), Small.getMagnitude());
    EXPECT_FLOAT_EQ(units::max(Negative, Big).getMagnitude(), Big.getMagnitude());
}