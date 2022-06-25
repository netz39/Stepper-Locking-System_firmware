#pragma once

#include "FadingAnimation.hpp"
#include "LedAnimationBase.hpp"

class DoorIsOpenAnimation : public LedAnimationBase
{
public:
    explicit DoorIsOpenAnimation(LedSegmentArray &ledData)
        : fadingAnimation(ledData), ledData(ledData){};

    void doAnimationStep() override
    {
        if (stateCounter >= 6)
        {
            for (size_t i = 0; i < NumberOfLedsInRing; i++)
                ledData[i] = Green * 0.25;

            setDelay(20.0_ms);
        }
        else
        {
            fadingAnimation.doAnimationStep();

            if (fadingAnimation.isAnimationFinished())
            {
                stateCounter++;
                updateState();
            }

            setDelay(fadingAnimation.getDelay());
        }
    }

protected:
    void resetInheritedAnimation() override
    {
        stateCounter = 0;
        fadingAnimation.setFadingTime(0.45_s);
        updateState();
    }

private:
    FadingAnimation fadingAnimation;
    LedSegmentArray &ledData;

    uint8_t stateCounter = 0;

    void updateState()
    {
        if (stateCounter >= 6)
        {
            animationIsFinished();
        }

        const auto TargetColor = (stateCounter % 2 == 0) ? Green * 0.6 : Green * 0.25;

        for (size_t i = 0; i < NumberOfLedsInRing; i++)
            fadingAnimation.targetLedData[i] = TargetColor;

        fadingAnimation.resetAnimation();
    }
};
