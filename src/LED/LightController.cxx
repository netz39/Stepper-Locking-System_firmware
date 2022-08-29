#include "FreeRTOS.h"
#include "helpers/freertos.hpp"
#include "task.h"

#include "LightController.hpp"
#include "sync.hpp"

#include <algorithm>
#include <climits>

using util::pwm_led::DualLedColor;
using util::wrappers::NotifyAction;

[[noreturn]] void LightController::taskMain()
{
    sync::waitForAll(sync::StateMachineStarted);

    while (true)
    {
        updateLightState();
        statusLed.updateState(xTaskGetTickCount());

        targetAnimation->doAnimationStep();
        ledDriver.sendBuffer(ledSegments1, ledSegments2);

        vTaskDelay(toOsTicks(targetAnimation->getDelay()));
    }
}

//--------------------------------------------------------------------------------------------------
void LightController::onSettingsUpdate()
{
    invertRotationDirection =
        settingsContainer.getValue<firmwareSettings::InvertRotationDirection, bool>();
}

//--------------------------------------------------------------------------------------------------
void LightController::notifySpiIsFinished()
{
    auto higherPriorityTaskWoken = pdFALSE;
    notifyFromISR(1, NotifyAction::SetBits, &higherPriorityTaskWoken);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

//--------------------------------------------------------------------------------------------------
void LightController::updateLightState()
{
    switch (stateMachine.getCurrentState())
    {
    case StateMachine::State::Opened:
        statusLed.setColor(DualLedColor::DarkGreen);
        targetAnimation = &doorIsOpenAnimation;
        break;

    case StateMachine::State::Opening:
        statusLed.setColorBlinking(DualLedColor::Green, 2.0_Hz);
        targetAnimation = &whirlingAnimation;
        whirlingAnimation.setWhirlingMode(WhirlingAnimation::WhirlingMode::Opening,
                                          invertRotationDirection);
        whirlingAnimation.setProgress(motorController.getProgress());
        break;

    case StateMachine::State::ManualOpening:
        statusLed.setColorBlinking(DualLedColor::Green, 1.0_Hz);
        targetAnimation = &whirlingAnimation;
        whirlingAnimation.setWhirlingMode(WhirlingAnimation::WhirlingMode::ManualOpening,
                                          invertRotationDirection);
        break;

    case StateMachine::State::Closed:
        statusLed.setColor(DualLedColor::DarkRed);
        targetAnimation = &doorIsClosedAnimation;
        break;

    case StateMachine::State::Closing:
        statusLed.setColorBlinking(DualLedColor::Red, 2.0_Hz);
        targetAnimation = &whirlingAnimation;
        whirlingAnimation.setWhirlingMode(WhirlingAnimation::WhirlingMode::Closing,
                                          invertRotationDirection);
        whirlingAnimation.setProgress(motorController.getProgress());
        break;

    case StateMachine::State::ManualClosing:
        statusLed.setColorBlinking(DualLedColor::Red, 1.0_Hz);
        targetAnimation = &whirlingAnimation;
        whirlingAnimation.setWhirlingMode(WhirlingAnimation::WhirlingMode::ManualClosing,
                                          invertRotationDirection);
        break;

    case StateMachine::State::Calibrating:
        statusLed.setColorBlinking(DualLedColor::Yellow, 2.0_Hz);
        targetAnimation = &showStatusAnimation;
        showStatusAnimation.showCalibration();
        break;

    case StateMachine::State::WantToClose:
    case StateMachine::State::RetryToClose:
        statusLed.setColorBlinking(DualLedColor::DarkRed, 0.5_Hz);
        targetAnimation = &doorShouldCloseAnimation;
        break;

    case StateMachine::State::Warning:
    default:
        statusLed.setColorBlinking(DualLedColor::Yellow, 3.0_Hz);
        targetAnimation = &showStatusAnimation;
        showStatusAnimation.showWarning();
        break;

    case StateMachine::State::FatalError:
        statusLed.setColorBlinking(DualLedColor::Red, 3.0_Hz);
        targetAnimation = &showStatusAnimation;
        showStatusAnimation.showCritical();
        break;
    }

    if (prevState != stateMachine.getCurrentState())
    {
        // prevent interruption of animation when switching from RetryToClose to WantToClose
        if (prevState != StateMachine::State::RetryToClose ||
            stateMachine.getCurrentState() != StateMachine::State::WantToClose)
        {
            targetAnimation->resetAnimation();
        }

        prevState = stateMachine.getCurrentState();
    }
}