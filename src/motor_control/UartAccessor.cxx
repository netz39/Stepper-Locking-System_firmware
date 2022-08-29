#include "UartAccessor.hpp"
#include "helpers/freertos.hpp"

void UartAccessor::beginTransaction()
{
    mutex.lock();
}

//--------------------------------------------------------------------------------------------------
void UartAccessor::endTransaction()
{
    mutex.unlock();
}

//--------------------------------------------------------------------------------------------------
bool UartAccessor::receive(uint8_t *buffer, uint16_t length)
{
    errorCondition = false;

    portENTER_CRITICAL();
    errorCondition = HAL_UART_Receive_DMA(uartHandle, buffer, length) != HAL_OK;
    portEXIT_CRITICAL();

    errorCondition = errorCondition || (binary.take(toOsTicks(Timeout)) == pdFALSE);

    bool returnValue = errorCondition;
    errorCondition = false;
    return !returnValue;
}

//--------------------------------------------------------------------------------------------------
bool UartAccessor::transmit(const uint8_t *data, uint16_t length)
{
    errorCondition = false;

    portENTER_CRITICAL();
    errorCondition =
        HAL_UART_Transmit_DMA(uartHandle, const_cast<uint8_t *>(data), length) != HAL_OK;
    portEXIT_CRITICAL();

    errorCondition = errorCondition || (binary.take(toOsTicks(Timeout)) == pdFALSE);

    bool returnValue = errorCondition;
    errorCondition = false;
    return !returnValue;
}

//--------------------------------------------------------------------------------------------------
void UartAccessor::signalTransferCompleteFromIsr()
{
    auto higherPrioTaskWoken = pdFALSE;
    errorCondition = false;
    binary.giveFromISR(&higherPrioTaskWoken);
    portYIELD_FROM_ISR(higherPrioTaskWoken);
}

//--------------------------------------------------------------------------------------------------
void UartAccessor::signalErrorFromIsr()
{
    auto higherPrioTaskWoken = pdFALSE;
    errorCondition = true;
    binary.giveFromISR(&higherPrioTaskWoken);
    portYIELD_FROM_ISR(higherPrioTaskWoken);
}

//--------------------------------------------------------------------------------------------------
void UartAccessor::halfDuplexSwitchToRx()
{
    portENTER_CRITICAL();
    HAL_HalfDuplex_EnableReceiver(uartHandle); // todo check hal errors
    portEXIT_CRITICAL();
}

//--------------------------------------------------------------------------------------------------
void UartAccessor::halfDuplexSwitchToTx()
{
    portENTER_CRITICAL();
    HAL_HalfDuplex_EnableTransmitter(uartHandle); // todo check hal errors
    portEXIT_CRITICAL();
}