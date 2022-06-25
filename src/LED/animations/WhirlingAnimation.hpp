#pragma once

#include "LedAnimationBase.hpp"

class WhirlingAnimation : public LedAnimationBase
{
public:
    explicit WhirlingAnimation(LedSegmentArray &ledData) : ledData(ledData){};

    void setOpening()
    {
        isOpening = true;
    }

    void setClosing()
    {
        isOpening = false;
    }

    void setClockwiseWhirling()
    {
        isClockwiseWhirling = true;
    }

    void setCounterClockwiseWhirling()
    {
        isClockwiseWhirling = false;
    }

    void setProgess(uint8_t valueInPercent)
    {
        progress = valueInPercent;
    }

    void doAnimationStep() override
    {
        for (size_t i = 0; i < NumberOfLedsPerRing; i++)
            ledData[i] = ColorOff;

        const uint8_t ProgessLedEnd = progress * NumberOfLedsInRing / 100;
        constexpr auto ProgessColor = Blue * 0.4;

        if (isClockwiseWhirling)
            for (uint8_t i = NumberOfLedsInRing - 1; i >= NumberOfLedsInRing - ProgessLedEnd; i--)
                ledData[i] = ProgessColor;
        else
            for (uint8_t i = 0; i < ProgessLedEnd; i++)
                ledData[i] = ProgessColor;

        const auto LedIndex1 = whirlIndex % NumberOfLedsInRing;
        const auto LedIndex2 = (whirlIndex + 1) % NumberOfLedsInRing;
        const auto LedIndex3 = (NumberOfLedsInRing / 2 + LedIndex1) % NumberOfLedsInRing;
        const auto LedIndex4 = (NumberOfLedsInRing / 2 + LedIndex2) % NumberOfLedsInRing;

        const auto WhirlingColor = (isOpening ? Green : Red) * 0.75;
        ledData[LedIndex1] = WhirlingColor;
        ledData[LedIndex2] = WhirlingColor;
        ledData[LedIndex3] = WhirlingColor;
        ledData[LedIndex4] = WhirlingColor;

        // progess simulation
        /*
        if (whirlIndex % (NumberOfLedsInRing / 32) == 0)
            if (++progress >= 100)
            {
                progress = 0;
                animationIsFinished();
                return;
            }
        */

        isClockwiseWhirling ? whirlIndex-- : whirlIndex++;

        setDelay(50.0_ms);
    }

protected:
    void resetInheritedAnimation() override
    {
        progress = 0;
        whirlIndex = 0;
    }

private:
    LedSegmentArray &ledData;

    size_t progress = 0;
    size_t whirlIndex = 0;

    bool isOpening = true;
    bool isClockwiseWhirling = true;
};
