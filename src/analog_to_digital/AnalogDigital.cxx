#include "FreeRTOS.h"
#include "adc.h"
#include "task.h"

#include "AnalogDigital.hpp"
#include "helpers/freertos.hpp"

void AnalogDigital::taskMain()
{
    static constexpr auto AdcTaskFrequency = 50.0_Hz;

    calibrateAdc();

    auto lastWakeTime = xTaskGetTickCount();

    while (true)
    {
        startConversion();
        waitUntilConversionFinished();

        calculateReferenceVoltage();
        updateFastLowpass(
            inputCurrent,
            physicalQuantityFromAdcResult(adcResults[InputCurrentRank], CurrentMeasurementFactor),
            SampleCount);

        const auto DropVoltage = physicalQuantityFromAdcResult(adcResults[MotorTemperatureRank], 1);
        updateFastLowpass(motorTemperature, calculateNtcTemperature(DropVoltage), SampleCount);

        // TODO:
        // MCU temperature

        vTaskDelayUntil(&lastWakeTime, toOsTicks(AdcTaskFrequency));
    }
}

//----------------------------------------------------------------------------------------------
void AnalogDigital::conversionCompleteCallback()
{
    auto higherPriorityTaskWoken = pdFALSE;
    notifyFromISR(1, eSetBits, &higherPriorityTaskWoken);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

//----------------------------------------------------------------------------------------------
void AnalogDigital::waitUntilConversionFinished()
{
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

//----------------------------------------------------------------------------------------------
void AnalogDigital::calibrateAdc()
{
    if (HAL_ADCEx_Calibration_Start(AdcPeripherie) != HAL_OK)
        __asm("bkpt");
}

//----------------------------------------------------------------------------------------------
void AnalogDigital::startConversion()
{
    HAL_ADC_Start_DMA(AdcPeripherie, reinterpret_cast<uint32_t *>(adcResults.data()),
                      TotalChannelCount);
}

//----------------------------------------------------------------------------------------------
void AnalogDigital::calculateReferenceVoltage()
{
    const auto VrefIntCalibration = *VREFINT_CAL_ADDR;
    constexpr Voltage CalibrationRefVoltage = 3.0_V;

    referenceVoltage = VrefIntCalibration * CalibrationRefVoltage / adcResults[VrefRank];
}

//----------------------------------------------------------------------------------------------
Temperature AnalogDigital::calculateNtcTemperature(const Voltage dropVoltage)
{
    const auto NtcResistance =
        (referenceVoltage * NtcSecondResistor - dropVoltage * NtcSecondResistor) / dropVoltage;

    const float LogValue =
        log((NtcResistance / NtcResistanceAtNominalTemperature).getMagnitude<double>());

    return Temperature{
        1 / ((1_ / NtcNominalTemperature).getMagnitude() + (1 / NtcBetaValue) * LogValue)};
}