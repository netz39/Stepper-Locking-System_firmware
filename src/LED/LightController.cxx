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

    auto lastWakeTime = xTaskGetTickCount();

    while (true)
    {
        updateLightState();

        statusLed.updateState(lastWakeTime);
        //  sendBuffer();

        // uint32_t notficationValue;
        static constexpr auto TaskFrequency = 50.0_Hz;
        // notifyWait(0, ULONG_MAX, &notficationValue, toOsTicks(TaskFrequency));
        vTaskDelayUntil(&lastWakeTime, toOsTicks(TaskFrequency));
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
inline void LightController::convertToGammaCorrectedColors()
{
    for (size_t i = 0; i < TotalNumberOfLeds; i++)
        ledSpiDatas[i].assignGammaCorrectedColor(ledSegments[i]);
}

//--------------------------------------------------------------------------------------------------
void LightController::sendBuffer()
{
    convertToGammaCorrectedColors();

    // see following links or details
    // https://cpldcpu.wordpress.com/2014/11/30/understanding-the-apa102-superled/
    // https://cpldcpu.wordpress.com/2016/12/13/sk9822-a-clone-of-the-apa102/

    sendStartFrame();

    HAL_SPI_Transmit_DMA(SpiDevice, reinterpret_cast<uint8_t *>(ledSpiDatas.data()),
                         TotalNumberOfLeds * sizeof(LedSpiData));
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
        break;

    case StateMachine::State::Opening:
        statusLed.setColorBlinking(DualLedColor::Green, 2.0_Hz);
        break;

    case StateMachine::State::Closed:
        statusLed.setColor(DualLedColor::DarkRed);
        break;

    case StateMachine::State::Closing:
        statusLed.setColorBlinking(DualLedColor::Red, 2.0_Hz);
        break;

    case StateMachine::State::Calibrating:
        statusLed.setColorBlinking(DualLedColor::Yellow, 2.0_Hz);
        break;

    case StateMachine::State::WantToClose:
        statusLed.setColorBlinking(DualLedColor::DarkRed, 0.5_Hz);
        break;

    default:
        statusLed.setColorBlinking(DualLedColor::Orange, 0.5_Hz);
        break;
    }
}