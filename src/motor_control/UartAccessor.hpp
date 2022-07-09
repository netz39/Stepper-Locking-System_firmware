#pragma once

#include "FreeRTOS.h"
#include "semphr.h"

#include "units/si/time.hpp"
#include "usart.h"

class UartAccessor
{
public:
    explicit UartAccessor(UART_HandleTypeDef *huart) : uartHandle{huart}
    {
        mutex = xSemaphoreCreateMutex();
        binary = xSemaphoreCreateBinary();
    }

    void beginTransaction();
    void endTransaction();

    void halfDuplexSwitchToRx();
    void halfDuplexSwitchToTx();

    bool receive(uint8_t *buffer, uint16_t length);
    bool transmit(const uint8_t *data, uint16_t length);

    void signalTransferCompleteFromIsr(BaseType_t *higherPrioTaskWoken = nullptr);
    void signalErrorFromIsr(BaseType_t *higherPrioTaskWoken = nullptr);

    static constexpr units::si::Time Timeout = 1.0_s;

private:
    UART_HandleTypeDef *uartHandle = nullptr;
    SemaphoreHandle_t mutex = nullptr;
    SemaphoreHandle_t binary = nullptr;
    bool errorCondition = false;
};
