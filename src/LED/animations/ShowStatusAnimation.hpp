#pragma once

#include "LedAnimationBase.hpp"

/// show various status
class ShowStatusAnimation : public LedAnimationBase
{
public:
    explicit ShowStatusAnimation(LedSegmentArray &ledData) : ledData(ledData){};

    void showWarning()
    {
        currentState = State::Warning;
    }

    void showCritical()
    {
        currentState = State::Critical;
    }

    void showCalibration()
    {
        currentState = State::Calibration;
    }

    void doAnimationStep() override
    {
        constexpr auto DistanceBetweenLeds = 4;

        BgrColor targetColor;

        switch (currentState)
        {
        case State::Calibration:
        case State::Warning:
            targetColor = Yellow;
            break;

        case State::Critical:
            targetColor = Red;
            break;
        }

        targetColor *= 0.6;

        for (size_t i = 0; i < NumberOfLedsPerRing; i++)
            ledData[i] = ColorOff;

        if (currentState == State::Calibration)
        {
            ledData[(counter) % NumberOfLedsInRing] = targetColor;
            ledData[(14 + counter) % NumberOfLedsInRing] = targetColor;
            ledData[(16 + counter) % NumberOfLedsInRing] = targetColor;
            ledData[(30 + counter) % NumberOfLedsInRing] = targetColor;
        }
        else
        {
            for (size_t i = 0; i < NumberOfLedsInRing; i += DistanceBetweenLeds)
                ledData[(i + counter) % NumberOfLedsInRing] = targetColor;
        }

        ++counter %= 2;

        setDelay(150.0_ms);
    }

protected:
    void resetInheritedAnimation() override
    {
        counter = 0;
    }

private:
    LedSegmentArray &ledData;

    enum class State
    {
        Warning,
        Critical,
        Calibration
    };

    State currentState = State::Warning;

    uint8_t counter = 0;
};
