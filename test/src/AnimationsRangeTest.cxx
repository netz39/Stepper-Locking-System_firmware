#include <gtest/gtest.h>

#include "src/LED/AnimationController.hpp"

constexpr auto UpdateCycles = 1000;

TEST(RangeTest, RainbowAnimation)
{
    LedSegmentArray testArray{};
    RainbowAnimation animationToTest{testArray};
    animationToTest.resetAnimation();

    for (int i = 0; i < UpdateCycles; i++)
        animationToTest.doAnimationStep();
}

TEST(RangeTest, DoorIsClosedAnimation)
{
    LedSegmentArray testArray{};
    DoorIsClosedAnimation animationToTest{testArray};
    animationToTest.resetAnimation();

    for (int i = 0; i < UpdateCycles; i++)
        animationToTest.doAnimationStep();
    // animationToTest.doAnimationStep();
}

TEST(RangeTest, DoorIsOpenAnimation)
{
    LedSegmentArray testArray{};
    DoorIsOpenAnimation animationToTest{testArray};
    animationToTest.resetAnimation();

    for (int i = 0; i < UpdateCycles; i++)
        animationToTest.doAnimationStep();
}

TEST(RangeTest, DoorShouldCloseAnimation)
{
    LedSegmentArray testArray{};
    DoorShouldCloseAnimation animationToTest{testArray};
    animationToTest.resetAnimation();

    for (int i = 0; i < UpdateCycles; i++)
        animationToTest.doAnimationStep();
}

TEST(RangeTest, ShowStatusAnimationWarning)
{
    LedSegmentArray testArray{};
    ShowStatusAnimation animationToTest{testArray};
    animationToTest.resetAnimation();
    animationToTest.showWarning();

    for (int i = 0; i < UpdateCycles; i++)
        animationToTest.doAnimationStep();
}

TEST(RangeTest, ShowStatusAnimationFatalError)
{
    LedSegmentArray testArray{};
    ShowStatusAnimation animationToTest{testArray};
    animationToTest.resetAnimation();
    animationToTest.showCritical();

    for (int i = 0; i < UpdateCycles; i++)
        animationToTest.doAnimationStep();
}

TEST(RangeTest, ShowStatusAnimationCalibration)
{
    LedSegmentArray testArray{};
    ShowStatusAnimation animationToTest{testArray};
    animationToTest.resetAnimation();
    animationToTest.showCalibration();

    for (int i = 0; i < UpdateCycles; i++)
        animationToTest.doAnimationStep();
}

TEST(RangeTest, TestAllColorsAnimation)
{
    LedSegmentArray testArray{};
    TestAllColorsAnimation animationToTest{testArray};
    animationToTest.resetAnimation();

    for (int i = 0; i < UpdateCycles; i++)
        animationToTest.doAnimationStep();
}

TEST(RangeTest, WhirlingAnimationOpen)
{
    LedSegmentArray testArray{};
    WhirlingAnimation animationToTest{testArray};
    animationToTest.resetAnimation();
    animationToTest.setClockwiseWhirling();
    animationToTest.setWhirlingMode(WhirlingAnimation::WhirlingMode::Opening);

    uint8_t progessCounter = 0;

    for (int i = 0; i < UpdateCycles; i++)
    {
        animationToTest.setProgess(progessCounter++);
        if (progessCounter > 100)
            progessCounter = 0;
        animationToTest.doAnimationStep();
    }

    animationToTest.setCounterClockwiseWhirling();
    progessCounter = 0;

    for (int i = 0; i < UpdateCycles; i++)
    {
        animationToTest.setProgess(progessCounter++);
        if (progessCounter > 100)
            progessCounter = 0;
        animationToTest.doAnimationStep();
    }
}

TEST(RangeTest, WhirlingAnimationClose)
{
    LedSegmentArray testArray{};
    WhirlingAnimation animationToTest{testArray};
    animationToTest.resetAnimation();
    animationToTest.setClockwiseWhirling();
    animationToTest.setWhirlingMode(WhirlingAnimation::WhirlingMode::Closing);

    uint8_t progessCounter = 0;

    for (int i = 0; i < UpdateCycles; i++)
    {
        animationToTest.setProgess(progessCounter++);
        if (progessCounter > 100)
            progessCounter = 0;
        animationToTest.doAnimationStep();
    }

    animationToTest.setCounterClockwiseWhirling();
    progessCounter = 0;

    for (int i = 0; i < UpdateCycles; i++)
    {
        animationToTest.setProgess(progessCounter++);
        if (progessCounter > 100)
            progessCounter = 0;
        animationToTest.doAnimationStep();
    }
}

TEST(RangeTest, WhirlingAnimationManualOpening)
{
    LedSegmentArray testArray{};
    WhirlingAnimation animationToTest{testArray};
    animationToTest.resetAnimation();
    animationToTest.setClockwiseWhirling();
    animationToTest.setWhirlingMode(WhirlingAnimation::WhirlingMode::ManualOpening);

    uint8_t progessCounter = 0;

    for (int i = 0; i < UpdateCycles; i++)
    {
        animationToTest.setProgess(progessCounter++);
        if (progessCounter > 100)
            progessCounter = 0;
        animationToTest.doAnimationStep();
    }

    animationToTest.setCounterClockwiseWhirling();
    progessCounter = 0;

    for (int i = 0; i < UpdateCycles; i++)
    {
        animationToTest.setProgess(progessCounter++);
        if (progessCounter > 100)
            progessCounter = 0;
        animationToTest.doAnimationStep();
    }
}

TEST(RangeTest, WhirlingAnimationManualClose)
{
    LedSegmentArray testArray{};
    WhirlingAnimation animationToTest{testArray};
    animationToTest.resetAnimation();
    animationToTest.setClockwiseWhirling();
    animationToTest.setWhirlingMode(WhirlingAnimation::WhirlingMode::ManualClosing);

    uint8_t progessCounter = 0;

    for (int i = 0; i < UpdateCycles; i++)
    {
        animationToTest.setProgess(progessCounter++);
        if (progessCounter > 100)
            progessCounter = 0;
        animationToTest.doAnimationStep();
    }

    animationToTest.setCounterClockwiseWhirling();
    progessCounter = 0;

    for (int i = 0; i < UpdateCycles; i++)
    {
        animationToTest.setProgess(progessCounter++);
        if (progessCounter > 100)
            progessCounter = 0;
        animationToTest.doAnimationStep();
    }
}
