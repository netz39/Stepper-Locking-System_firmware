#include "FreeRTOS.h"
#include "helpers/freertos.hpp"
#include "task.h"

#include "LightController.hpp"
#include "sync.hpp"

#include <algorithm>
#include <climits>

using util::pwm_led::DualLedColor;

void LightController::taskMain()
{
    RedChannel.startPwmTimer();
    GreenChannel.startPwmTimer();

    sync::waitForAll(sync::StateMachineStarted);

    while (true)
    {
        updateLightState();
        statusLed.updateState(xTaskGetTickCount());

        targetAnimation->doAnimationStep();
        sendBuffer();

        vTaskDelay(toOsTicks(targetAnimation->getDelay()));
    }
}

//--------------------------------------------------------------------------------------------------
void LightController::onSettingsUpdate()
{
    invertRotationDirection =
        settingsContainer.getValue<firmwareSettings::InvertRotationDirection>();
}

//--------------------------------------------------------------------------------------------------
inline void LightController::sendStartFrame()
{
    uint32_t startFrame = 0;

    HAL_SPI_Transmit_DMA(SpiDevice, reinterpret_cast<uint8_t *>(&startFrame), sizeof(startFrame));
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

//--------------------------------------------------------------------------------------------------
inline void LightController::convertToGammaCorrectedColors(LedSegmentArray &source,
                                                           LedSpiDataArray &destination)
{
    for (size_t i = 0; i < destination.size(); i++)
        destination[i].assignGammaCorrectedColor(source[i]);
}

//--------------------------------------------------------------------------------------------------
void LightController::sendBuffer()
{
    convertToGammaCorrectedColors(ledSegments1, ledSpiData1);
    convertToGammaCorrectedColors(ledSegments2, ledSpiData2);

    // see following links or details
    // https://cpldcpu.wordpress.com/2014/11/30/understanding-the-apa102-superled/
    // https://cpldcpu.wordpress.com/2016/12/13/sk9822-a-clone-of-the-apa102/

    sendStartFrame();

    HAL_SPI_Transmit_DMA(SpiDevice, reinterpret_cast<uint8_t *>(ledSpiData1.data()),
                         ledSpiData1.size() * sizeof(LedSpiData));
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    HAL_SPI_Transmit_DMA(SpiDevice, reinterpret_cast<uint8_t *>(ledSpiData2.data()),
                         ledSpiData2.size() * sizeof(LedSpiData));
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    HAL_SPI_Transmit_DMA(SpiDevice, endFrames.data(), NumberOfEndFrames);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

//--------------------------------------------------------------------------------------------------
void LightController::notifySpiIsFinished()
{
    auto higherPriorityTaskWoken = pdFALSE;
    notifyFromISR(1, eSetBits, &higherPriorityTaskWoken);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

//--------------------------------------------------------------------------------------------------
void LightController::updateLightState()
{
    switch (stateMaschine.currentState)
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
        whirlingAnimation.setProgess(motorController.getProgress());
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
        whirlingAnimation.setProgess(motorController.getProgress());
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

    if (prevState != stateMaschine.currentState)
    {
        // prevent interruption of animation when switching from RetryToClose to WantToClose
        if (prevState != StateMachine::State::RetryToClose ||
            stateMaschine.currentState != StateMachine::State::WantToClose)
        {
            targetAnimation->resetAnimation();
        }

        prevState = stateMaschine.currentState;
    }
}