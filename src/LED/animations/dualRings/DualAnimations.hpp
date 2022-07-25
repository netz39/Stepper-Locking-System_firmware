#pragma once

#include "LED/animations/ShowStatusAnimation.hpp"
#include "LED/animations/WhirlingAnimation.hpp"

template <class T>
class DualAnimations : public LedAnimationBase
{
public:
    DualAnimations(std::array<BgrColor, NumberOfLedsPerRing> &firstRingSegments,
                   std::array<BgrColor, NumberOfLedsPerRing> &secondRingSegments)
        : firstRing(firstRingSegments), secondRing(secondRingSegments)
    {
    }
    ~DualAnimations() override = default;

    // only for whirling animations, see definitions below this class
    void setWhirlingMode(WhirlingAnimation::WhirlingMode mode,
                         bool invertRotationDirection = false);
    void setProgress(uint8_t valueInPercent);

    // only for warning animations, see definitions below this class
    void showWarning();
    void showCritical();
    void showCalibration();

    void doAnimationStep() override
    {
        firstRing.doAnimationStep();
        secondRing.doAnimationStep();

        setDelay(firstRing.getDelay());

        if (firstRing.isAnimationFinished())
            animationIsFinished();
    }

protected:
    void resetInheritedAnimation() override
    {
        firstRing.resetAnimation();
        secondRing.resetAnimation();
    }

private:
    T firstRing;
    T secondRing;
};

template <>
inline void DualAnimations<WhirlingAnimation>::setWhirlingMode(WhirlingAnimation::WhirlingMode mode,
                                                               bool invertRotationDirection)
{
    firstRing.setWhirlingMode(mode);
    secondRing.setWhirlingMode(mode);

    const bool IsOpening = mode == WhirlingAnimation::WhirlingMode::Opening ||
                           mode == WhirlingAnimation::WhirlingMode::ManualOpening;

    if (IsOpening == invertRotationDirection)
    {
        firstRing.setCounterClockwiseWhirling();
        secondRing.setClockwiseWhirling();
    }
    else
    {
        firstRing.setClockwiseWhirling();
        secondRing.setCounterClockwiseWhirling();
    }
}

template <>
inline void DualAnimations<WhirlingAnimation>::setProgress(uint8_t valueInPercent)
{
    firstRing.setProgess(valueInPercent);
    secondRing.setProgess(valueInPercent);
}

template <>
inline void DualAnimations<ShowStatusAnimation>::showWarning()
{
    firstRing.showWarning();
    secondRing.showWarning();
}

template <>
inline void DualAnimations<ShowStatusAnimation>::showCritical()
{
    firstRing.showCritical();
    secondRing.showCritical();
}

template <>
inline void DualAnimations<ShowStatusAnimation>::showCalibration()
{
    firstRing.showCalibration();
    secondRing.showCalibration();
}