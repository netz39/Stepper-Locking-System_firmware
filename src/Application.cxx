#include "FreeRTOS.h"
#include "main.h"
#include "task.h"

#include "Application.hpp"
#include "wrappers/Task.hpp"

#include <memory>

extern "C" void StartDefaultTask(void *) // NOLINT
{
    static auto app = std::make_unique<Application>();
    app->run();

    SafeAssert(false); // this line should be never reached
}

//--------------------------------------------------------------------------------------------------
Application::Application()
{
    // Delegated Singleton, see getApplicationInstance() for further explanations
    SafeAssert(instance == nullptr);
    instance = this;

    HAL_StatusTypeDef result = HAL_OK;

    // use capture-less lambdas [](...HandleTypeDef*){} as callbacks

    // clang-format off
    result = HAL_ADC_RegisterCallback(AdcPeripherie, HAL_ADC_CONVERSION_COMPLETE_CB_ID,
        [](ADC_HandleTypeDef *){ getApplicationInstance().analogDigital.conversionCompleteCallback(); });

    // SPI callback for addressable LEDs
    result = HAL_SPI_RegisterCallback(&lightController.getSPIPeripheral(), HAL_SPI_TX_COMPLETE_CB_ID,
        [](SPI_HandleTypeDef *){ getApplicationInstance().lightController.notifySpiIsFinished(); });
    

    // EEPROM callbacks
    result = HAL_I2C_RegisterCallback(EepromBus, HAL_I2C_MASTER_TX_COMPLETE_CB_ID, 
        [](I2C_HandleTypeDef *){ getApplicationInstance().eepromBusAccessor.signalTransferCompleteFromIsr(); });

    result = HAL_I2C_RegisterCallback(EepromBus, HAL_I2C_MASTER_RX_COMPLETE_CB_ID,
        [](I2C_HandleTypeDef *){ getApplicationInstance().eepromBusAccessor.signalTransferCompleteFromIsr(); });

    result = HAL_I2C_RegisterCallback(EepromBus, HAL_I2C_ERROR_CB_ID, 
        [](I2C_HandleTypeDef *){ getApplicationInstance().eepromBusAccessor.signalErrorFromIsr(); });

    // TMC UART callbacks
    result = HAL_UART_RegisterCallback(TmcUartPeripherie, HAL_UART_TX_COMPLETE_CB_ID,
        [](UART_HandleTypeDef *){ getApplicationInstance().uartAccessorTmc.signalTransferCompleteFromIsr(); });

    result = HAL_UART_RegisterCallback(TmcUartPeripherie, HAL_UART_RX_COMPLETE_CB_ID,
        [](UART_HandleTypeDef *){ getApplicationInstance().uartAccessorTmc.signalTransferCompleteFromIsr(); });

    result = HAL_UART_RegisterCallback(TmcUartPeripherie, HAL_UART_ERROR_CB_ID, 
        [](UART_HandleTypeDef *){ getApplicationInstance().uartAccessorTmc.signalErrorFromIsr(); });
    // clang-format on

    SafeAssert(result == HAL_OK);
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