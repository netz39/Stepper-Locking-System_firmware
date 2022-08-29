#pragma once

#include <wrappers/BinarySemaphore.hpp>
#include <wrappers/Mutex.hpp>

#include "units/si/time.hpp"
#include "usart.h"

class UartAccessor
{
public:
    explicit UartAccessor(UART_HandleTypeDef *huart) : uartHandle{huart} {};

    void beginTransaction();
    void endTransaction();

    void halfDuplexSwitchToRx();
    void halfDuplexSwitchToTx();

    bool receive(uint8_t *buffer, uint16_t length);
    bool transmit(const uint8_t *data, uint16_t length);

    void signalTransferCompleteFromIsr();
    void signalErrorFromIsr();

    static constexpr units::si::Time Timeout = 500.0_ms;

private:
    UART_HandleTypeDef *uartHandle = nullptr;
    util::wrappers::Mutex mutex{};
    util::wrappers::BinarySemaphore binary{};
    volatile bool errorCondition = false;
};
