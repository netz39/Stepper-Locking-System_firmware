#pragma once

#include <array>
#include <cstddef>

#include "LED/LedDataTypes.hpp"
#include "units/si/time.hpp"

/// abstract class where animation implemention should derives
class LedAnimationBase
{
public:
    virtual ~LedAnimationBase() = default;

    void resetAnimation()
    {
        finished = false;
        resetInheritedAnimation();
    }

    virtual void doAnimationStep() = 0;

    [[nodiscard]] units::si::Time getDelay() const
    {
        return refreshDelay;
    }

    [[nodiscard]] bool isAnimationFinished() const
    {
        return finished;
    }

protected:
    void setDelay(units::si::Time delayTime)
    {
        refreshDelay = delayTime;
    }

    /// Each derived class should implement the reset process
    virtual void resetInheritedAnimation() = 0;

    void animationIsFinished()
    {
        finished = true;
    }

private:
    units::si::Time refreshDelay{0.0};
    bool finished = false;
};