#pragma once

#include "LedAnimationBase.hpp"
#include <cstring>

class FadingAnimation : public LedAnimationBase
{
public:
    explicit FadingAnimation(LedSegmentArray &ledData) : ledData(ledData){};
    ~FadingAnimation() override = default;

    void doAnimationStep() override
    {
        if (numberOfSteps == 0 || factor == 0)
        {
            animationIsFinished();
            return;
        }

        // apply difference multiplied by factor to current data
        for (uint32_t i = 0; i < NumberOfLedsPerRing; i++)
            ledData[i] = targetLedData[i] + (factor * diffLedData[i]) / numberOfSteps;

        if (factor == 0)
        {
            animationIsFinished();
            return;
        }

        factor--;
    }

    void setFadingTime(units::si::Time fadingTime)
    {
        this->fadingTime = fadingTime;
    }

    void setTargetLedData(LedSegmentArray newData)
    {
        std::memcpy(targetLedData.data(), newData.data(), NumberOfLedsPerRing * sizeof(BgrColor));
    }

    std::array<BgrColor, NumberOfLedsPerRing> targetLedData;

protected:
    void resetInheritedAnimation() override
    {
        setDelay(RefreshTime);

        numberOfSteps = (fadingTime / RefreshTime).getMagnitude<uint8_t>();
        factor = numberOfSteps - 1;

        // calc difference between current and target data
        for (uint32_t i = 0; i < NumberOfLedsPerRing; i++)
            diffLedData[i] = ledData[i] - targetLedData[i];
    }

private:
    static constexpr auto RefreshTime = 10.0_ms;

    LedSegmentArray &ledData;

    std::array<BgrColorDiff, NumberOfLedsPerRing> diffLedData;

    uint8_t numberOfSteps = 0;
    uint8_t factor = 0;
    units::si::Time fadingTime{0.0};
};