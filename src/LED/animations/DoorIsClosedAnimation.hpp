#pragma once

#include "FadingAnimation.hpp"
#include "LedAnimationBase.hpp"

class DoorIsClosedAnimation : public LedAnimationBase
{
public:
    explicit DoorIsClosedAnimation(LedSegmentArray &ledData) : ledData(ledData)
    {
    }
    ~DoorIsClosedAnimation() override = default;

    void doAnimationStep() override
    {
        if (stateCounter == 0)
        {
            for (uint8_t i = LedRightIndex; i <= std::min(runningLedIndex, LedLastIndex); i++)
                ledData[i] = Red * 0.25;

            if (runningLedIndex > LedLastIndex)
            {
                ledData[LedLeftIndex] = Red;

                stateCounter++;
                runningLedIndex--;
            }
            else
            {
                ledData[runningLedIndex] = Red;
                runningLedIndex++;
            }
        }

        else if (stateCounter == 1)
        {
            for (uint8_t i = LedRightIndex; i <= LedLastIndex; i++)
                ledData[i] = Red * 0.25;
            ledData[LedLeftIndex] = Red * 0.25;

            ledData[runningLedIndex] = Red;

            if (runningLedIndex == LedRightIndex)
                stateCounter++;

            else
                runningLedIndex--;
        }

        else if (stateCounter == 2)
        {
            for (uint8_t i = LedRightIndex; i <= LedLastIndex; i++)
                ledData[i] = Red * 0.25;

            ledData[LedLeftIndex] = Red * 0.25;

            animationIsFinished();
        }

        setDelay(30.0_ms);
    }

protected:
    void resetInheritedAnimation() override
    {
        stateCounter = 0;
        runningLedIndex = LedRightIndex;

        for (uint8_t i = 0; i < NumberOfLedsInRing; i++)
            ledData[i] = ColorOff;
    }

private:
    LedSegmentArray &ledData;

    uint8_t stateCounter = 0;

    static constexpr uint8_t LedLeftIndex = 15;
    static constexpr uint8_t LedLastIndex = NumberOfLedsPerRing - 1;
    static constexpr uint8_t LedRightIndex = NumberOfLedsInRing - 1;

    uint8_t runningLedIndex = LedRightIndex;
};
