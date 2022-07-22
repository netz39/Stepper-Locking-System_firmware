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
    // Delegated Singleton, see getApplicationInstance() for further explanations
    SafeAssert(instance == nullptr);
    instance = this;

    HAL_ADC_RegisterCallback(AnalogDigital::AdcPeripherie, HAL_ADC_CONVERSION_COMPLETE_CB_ID,
                             &adcConversionCompleteCallback);

    // SPI callback for addressable LEDs
    HAL_SPI_RegisterCallback(&lightController.getSPIPeripheral(), HAL_SPI_TX_COMPLETE_CB_ID,
                             &ledSpiCallback);

    // EEPROM callbacks
    HAL_I2C_RegisterCallback(EepromBus, HAL_I2C_MASTER_TX_COMPLETE_CB_ID, &i2cMasterCmpltCallback);
    HAL_I2C_RegisterCallback(EepromBus, HAL_I2C_MASTER_RX_COMPLETE_CB_ID, &i2cMasterCmpltCallback);
    HAL_I2C_RegisterCallback(EepromBus, HAL_I2C_ERROR_CB_ID, &i2cErrorCallback);

    // TMC UART callbacks
    HAL_UART_RegisterCallback(TmcUartPeripherie, HAL_UART_TX_COMPLETE_CB_ID, &uartTmcCmpltCallback);
    HAL_UART_RegisterCallback(TmcUartPeripherie, HAL_UART_RX_COMPLETE_CB_ID, &uartTmcCmpltCallback);
    HAL_UART_RegisterCallback(TmcUartPeripherie, HAL_UART_ERROR_CB_ID, &uartTmcErrorCallback);
}

//--------------------------------------------------------------------------------------------------
[[noreturn]] void Application::run()
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
    // Not constructing Application in this singleton, to avoid bugs where something tries to
    // access this function, while application constructs which will cause infinite recursion
    return *instance;
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
void Application::i2cMasterCmpltCallback(I2C_HandleTypeDef *)
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
void Application::uartTmcCmpltCallback(UART_HandleTypeDef *)
{
    auto higherPrioTaskWoken = pdFALSE;
    getApplicationInstance().uartAccessorTmc.signalTransferCompleteFromIsr(&higherPrioTaskWoken);
    portYIELD_FROM_ISR(higherPrioTaskWoken);
}

//--------------------------------------------------------------------------------------------------
void Application::uartTmcErrorCallback(UART_HandleTypeDef *)
{
    auto higherPrioTaskWoken = pdFALSE;
    getApplicationInstance().uartAccessorTmc.signalErrorFromIsr(&higherPrioTaskWoken);
    portYIELD_FROM_ISR(higherPrioTaskWoken);
}

//--------------------------------------------------------------------------------------------------
extern "C" void StartDefaultTask(void *) // NOLINT
{
    static auto app = std::make_unique<Application>();
    app->run();

    __asm("bkpt"); // this line should be never reached
}