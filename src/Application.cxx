#include "FreeRTOS.h"
#include "task.h"

#include "gpio.h"
#include "main.h"

#include "Application.hpp"
#include "TeensyStep/src/TeensyStep.h"
#include "motor_control/TMC2209.hpp"
#include "wrappers/Task.hpp"

#include <memory>

Application::Application()
{
    HAL_ADC_RegisterCallback(AnalogDigital::AdcPeripherie, HAL_ADC_CONVERSION_COMPLETE_CB_ID,
                             &adcConversionCompleteCallback);

    // SPI callback for addressable LEDs
    HAL_SPI_RegisterCallback(LightController::SpiDevice, HAL_SPI_TX_COMPLETE_CB_ID,
                             &ledSpiCallback);

    // EEPROM callbacks
    HAL_I2C_RegisterCallback(EepromBus, HAL_I2C_MASTER_TX_COMPLETE_CB_ID, &i2cMasterTxCallback);
    HAL_I2C_RegisterCallback(EepromBus, HAL_I2C_MASTER_RX_COMPLETE_CB_ID, &i2cMasterRxCallback);
    HAL_I2C_RegisterCallback(EepromBus, HAL_I2C_ERROR_CB_ID, &i2cErrorCallback);
}

//--------------------------------------------------------------------------------------------------
void Application::run()
{
    util::wrappers::Task::applicationIsReadyStartAllTasks();
    while (true)
    {
        vTaskDelay(portMAX_DELAY);
    }
}

//--------------------------------------------------------------------------------------------------
Application &Application::getApplicationInstance()
{
    static auto app = std::make_unique<Application>();
    return *app;
}

//--------------------------------------------------------------------------------------------------
void Application::adcConversionCompleteCallback(ADC_HandleTypeDef *)
{
    getApplicationInstance().analogDigital.conversionCompleteCallback();
}

//--------------------------------------------------------------------------------------------------
void Application::ledSpiCallback(SPI_HandleTypeDef *)
{
    getApplicationInstance().lightController.notifySpiIsFinished();
}

//--------------------------------------------------------------------------------------------------
void Application::i2cMasterTxCallback(I2C_HandleTypeDef *)
{
    auto higherPrioTaskWoken = pdFALSE;
    getApplicationInstance().eepromBusAccessor.signalTransferCompleteFromIsr(&higherPrioTaskWoken);
    portYIELD_FROM_ISR(higherPrioTaskWoken);
}

//--------------------------------------------------------------------------------------------------
void Application::i2cMasterRxCallback(I2C_HandleTypeDef *)
{
    auto higherPrioTaskWoken = pdFALSE;
    getApplicationInstance().eepromBusAccessor.signalTransferCompleteFromIsr(&higherPrioTaskWoken);
    portYIELD_FROM_ISR(higherPrioTaskWoken);
}

//--------------------------------------------------------------------------------------------------
void Application::i2cErrorCallback(I2C_HandleTypeDef *)
{
    auto higherPrioTaskWoken = pdFALSE;
    getApplicationInstance().eepromBusAccessor.signalErrorFromIsr(&higherPrioTaskWoken);
    portYIELD_FROM_ISR(higherPrioTaskWoken);
}

//--------------------------------------------------------------------------------------------------
extern "C" void StartDefaultTask(void *) // NOLINT
{

    auto &app = Application::getApplicationInstance();
    app.run();

    __asm("bkpt"); // this line should be never reached
}