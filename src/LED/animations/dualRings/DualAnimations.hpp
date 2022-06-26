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

    // only for whirling animations, see definitions below this class
    void setOpening();
    void setClosing();
    void setProgess(uint8_t valueInPercent);

    // only for warning animations, see definitions below this class
    void showWarning();
    void showCritical();

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
inline void DualAnimations<WhirlingAnimation>::setOpening()
{
    firstRing.setOpening();
    secondRing.setOpening();

    firstRing.setCounterClockwiseWhirling();
    secondRing.setClockwiseWhirling();
}

template <>
inline void DualAnimations<WhirlingAnimation>::setClosing()
{
    firstRing.setClosing();
    secondRing.setClosing();

    firstRing.setClockwiseWhirling();
    secondRing.setCounterClockwiseWhirling();
}

template <>
inline void DualAnimations<WhirlingAnimation>::setProgess(uint8_t valueInPercent)
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