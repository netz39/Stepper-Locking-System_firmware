#include "FreeRTOS.h"
#include "helpers/freertos.hpp"
#include "task.h"

#include "LightController.hpp"

#include <algorithm>
#include <climits>

using util::pwm_led::DualLedColor;

void LightController::taskMain()
{
    RedChannel.startPwmTimer();
    GreenChannel.startPwmTimer();

    // wait for state machine is started
    vTaskDelay(toOsTicks(550.0_ms));

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
        whirlingAnimation.setClockwiseWhirling();
        whirlingAnimation.setOpening();
        // whirlingAnimation.setProgess();
        break;

    case StateMachine::State::Closed:
        statusLed.setColor(DualLedColor::DarkRed);
        targetAnimation = &doorIsClosedAnimation;
        break;

    case StateMachine::State::Closing:
        statusLed.setColorBlinking(DualLedColor::Red, 2.0_Hz);
        targetAnimation = &whirlingAnimation;
        whirlingAnimation.setCounterClockwiseWhirling();
        whirlingAnimation.setClosing();
        // whirlingAnimation.setProgess();
        break;

    case StateMachine::State::Calibrating:
        statusLed.setColorBlinking(DualLedColor::Yellow, 2.0_Hz);
        targetAnimation = &showStatusAnimation;
        showStatusAnimation.showCalibration();
        break;

    case StateMachine::State::WantToClose:
        statusLed.setColorBlinking(DualLedColor::DarkRed, 0.5_Hz);
        targetAnimation = &doorShouldCloseAnimation;
        break;

    default:
        statusLed.setColorBlinking(DualLedColor::Orange, 0.5_Hz);
        targetAnimation = &showStatusAnimation;
        showStatusAnimation.showCritical();
        break;
    }

    if (prevState != stateMaschine.currentState)
    {
        targetAnimation->resetAnimation();
        prevState = stateMaschine.currentState;
    }
}