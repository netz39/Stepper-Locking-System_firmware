#pragma once

#include "FadingAnimation.hpp"
#include "LedAnimationBase.hpp"

class DoorShouldCloseAnimation : public LedAnimationBase
{
public:
    explicit DoorShouldCloseAnimation(LedSegmentArray &ledData) : ledData(ledData){};

    void doAnimationStep() override
    {
        const auto TargetColor = Green * 0.25;
        for (size_t i = 0; i < NumberOfLedsInRing; i++)
            ledData[i] = TargetColor;

        for (size_t i = NumberOfLedsInRing; i < NumberOfLedsPerRing; i++)
            ledData[i] = ColorOff;

        if (!runningBack)
        {
            if (runningLedIndex > LedLastIndex)
            {
                ledData[LedLeftIndex] = Red;

                runningBack = true;
                runningLedIndex--;
            }
            else
            {
                ledData[runningLedIndex] = Red;
                runningLedIndex++;
            }
        }

        else
        {
            ledData[runningLedIndex] = Red;

            if (runningLedIndex == LedRightIndex)
            {
                runningBack = false;
                runningLedIndex++;
            }

            else
                runningLedIndex--;
        }

        setDelay(60.0_ms);
    }

protected:
    void resetInheritedAnimation() override
    {
        counter = 0;
        runningLedIndex = LedRightIndex;
        runningBack = false;
    }

private:
    LedSegmentArray &ledData;

    static constexpr uint8_t LedLeftIndex = 15;
    static constexpr uint8_t LedLastIndex = NumberOfLedsPerRing - 1;
    static constexpr uint8_t LedRightIndex = NumberOfLedsInRing - 1;

    uint8_t runningLedIndex = LedRightIndex;
    bool runningBack = false;
    uint8_t counter = 0;
};
