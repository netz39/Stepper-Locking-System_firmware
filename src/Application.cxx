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
    HAL_SPI_RegisterCallback(LightController::SpiDevice, HAL_SPI_TX_COMPLETE_CB_ID,
                             &ledSpiCallback);
                             
    lightController.statusLed.setColor(util::pwm_led::DualLedColor::Yellow);
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
void Application::ledSpiCallback(SPI_HandleTypeDef *hspi)
{
    getApplicationInstance().lightController.notifySpiIsFinished();
}

//--------------------------------------------------------------------------------------------------
extern "C" void StartDefaultTask(void *) // NOLINT
{

    auto &app = Application::getApplicationInstance();
    app.run();

    __asm("bkpt"); // this line should be never reached
}