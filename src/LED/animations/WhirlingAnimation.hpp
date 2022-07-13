#pragma once

#include "LedAnimationBase.hpp"

class WhirlingAnimation : public LedAnimationBase
{
public:
    enum class WhirlingMode
    {
        Opening,
        Closing,
        ManualOpening,
        ManualClosing
    };

    explicit WhirlingAnimation(LedSegmentArray &ledData) : ledData(ledData){};

    void setWhirlingMode(WhirlingMode newMode)
    {
        mode = newMode;
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

        // progess bar only at normal opening and closing modes
        if (mode == WhirlingMode::Opening || mode == WhirlingMode::Closing)
        {
            const uint8_t ProgessLedEnd = progress * NumberOfLedsInRing / 100;
            constexpr auto ProgessColor = Blue * 0.25;

            if (isClockwiseWhirling)
                for (uint8_t i = 0; i < ProgessLedEnd; i++)
                    ledData[NumberOfLedsInRing - 1 - i] = ProgessColor;
            else
                for (uint8_t i = 0; i < ProgessLedEnd; i++)
                    ledData[i] = ProgessColor;
        }

        const bool IsOpening = mode == WhirlingMode::Opening || mode == WhirlingMode::ManualOpening;
        const bool IsNormalMode = mode == WhirlingMode::Opening || mode == WhirlingMode::Closing;
        const auto WhirlingColor = (IsOpening ? Green : Red) * 0.75;
        const auto WarningColor = Yellow * 0.6f;

        const auto LedIndex1 = whirlIndex % NumberOfLedsInRing;
        const auto LedIndex2 = (whirlIndex + 1) % NumberOfLedsInRing;
        const auto LedIndex3 = (NumberOfLedsInRing / 2 + LedIndex1) % NumberOfLedsInRing;
        const auto LedIndex4 = (NumberOfLedsInRing / 2 + LedIndex2) % NumberOfLedsInRing;

        ledData[LedIndex1] = WhirlingColor;
        ledData[LedIndex2] = WhirlingColor;
        ledData[LedIndex3] = IsNormalMode ? WhirlingColor : WarningColor;
        ledData[LedIndex4] = IsNormalMode ? WhirlingColor : WarningColor;

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

    WhirlingMode mode = WhirlingMode::Opening;
    bool isClockwiseWhirling = true;
};
