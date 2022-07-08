#pragma once
#if defined(STM32G0B1xx)
#include "HardwareTimer.h"
#include "Port.h"
#include <functional>
#include <stm32g0b1xx.h>

#include "../TF_Handler.h"

#define MAX_TIMERS 3
class TimerField
{
public:
    inline TimerField(TeensyStep::TF_Handler *_handler);

    inline bool begin();
    inline void end();
    inline void endAfterPulse(); // not nice look for better solution

    inline void stepTimerStart();
    inline void stepTimerStop();
    inline void setStepFrequency(unsigned f);
    inline unsigned getStepFrequency() const
    {
        return stepTimer.getTimerClkFreq() / stepTimer.getPrescaleFactor() /
               stepTimer.getOverflow();
    }
    inline bool stepTimerIsRunning() const
    {
        return stepTimerRunning;
    }

    inline void accTimerStart()
    {
        accTimer.resume();
        accTimer.refresh();
    }
    inline void accTimerStop()
    {
        accTimer.pause();
    }
    inline void setAccUpdatePeriod(unsigned period);

    inline void triggerDelay();
    inline void setPulseWidth(unsigned pulseWidth);

protected:
    static int instances;
    static TIM_TypeDef *timer_mapping[MAX_TIMERS];
    TeensyStep::TF_Handler *handler;
    HardwareTimer stepTimer;
    HardwareTimer accTimer;
    HardwareTimer pulseTimer;
    volatile bool stepTimerRunning;
    bool lastPulse = false;

    TIM_TypeDef *get_timer()
    {
        return instances >= MAX_TIMERS ? TIM1 : timer_mapping[instances++];
    }
};
// TODO:
// * Known bug: Second interrupt always happens after 20ms, no mather what acceleration is set. This
// causes all other interrupts to be slightly delayed. With default acceleration the second
// interrupt happens after 20ms instead of after 11.6ms. All acceleration interrupts are shifted by
// one step,
//
// Example with 10 steps(ms, offset to each  other):
//  Teensy35 has steps: 0, 11.6, 9.0,  7.6, 6.6, 7.6, 9.0, 11.6, 20.0, 20.0
//  stm32f4 has steps : 0, 20.0, 11.6, 9.0, 7.6, 6.6, 7.5,  9.0, 11.6, 20.0

// * Maximum 4 instances can be used, if more than four are initialized the first will break
// as its interrupt is reused.
// IMPLEMENTATION ====================================================================

TimerField::TimerField(TeensyStep::TF_Handler *_handler)
    : stepTimer(get_timer()), accTimer(get_timer()), pulseTimer(get_timer()),
      stepTimerRunning(false)
{
    handler = _handler;
    stepTimer.attachInterrupt([this] { handler->stepTimerISR(); });
    accTimer.attachInterrupt([this] { handler->accTimerISR(); });

    pulseTimer.attachInterrupt(
        [this]
        {
            handler->pulseTimerISR();
            this->pulseTimer.pause();
            if (lastPulse)
            {
                end();
            }
        }); // one-shot mode
}

void TimerField::stepTimerStart()
{
    stepTimer.resume();
    // Force update on the step timer registers as it might be slow(>50ms sometimes) to start
    // otherwise.
    stepTimer.refresh();
    stepTimerRunning = true;
}

void TimerField::stepTimerStop()
{
    stepTimer.pause();
    stepTimerRunning = false;
}

void TimerField::setAccUpdatePeriod(unsigned period)
{
    accTimer.setOverflow(period, MICROSEC_FORMAT);
}

void TimerField::triggerDelay()
{
    pulseTimer.resume();
}

void TimerField::setPulseWidth(unsigned pulseWidth)
{
    pulseTimer.setOverflow(pulseWidth, MICROSEC_FORMAT);
}

void TimerField::setStepFrequency(unsigned f)
{
    f == 0 ? end() : stepTimer.setOverflow(f, HERTZ_FORMAT);
}

bool TimerField::begin()
{
    pulseTimer.setPreloadEnable(false);
    lastPulse = false;
    return true;
}

void TimerField::end()
{
    stepTimer.pause();
    accTimer.pause();
    pulseTimer.pause();
    stepTimerRunning = false;
}

void TimerField::endAfterPulse()
{
    lastPulse = true;
}

#endif
