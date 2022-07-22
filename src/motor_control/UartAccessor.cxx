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
    HAL_UART_Receive_DMA(uartHandle, buffer, length);
    portEXIT_CRITICAL();
    errorCondition = (binary.take(toOsTicks(Timeout)) == pdFALSE);

    bool returnValue = errorCondition;
    errorCondition = false;
    return !returnValue;
}

//--------------------------------------------------------------------------------------------------
bool UartAccessor::transmit(const uint8_t *data, uint16_t length)
{
    errorCondition = false;

    portENTER_CRITICAL();
    HAL_UART_Transmit_DMA(uartHandle, const_cast<uint8_t *>(data), length);
    portEXIT_CRITICAL();
    errorCondition = (binary.take(toOsTicks(Timeout)) == pdFALSE);

    bool returnValue = errorCondition;
    errorCondition = false;
    return !returnValue;
}

//--------------------------------------------------------------------------------------------------
void UartAccessor::signalTransferCompleteFromIsr(BaseType_t *higherPrioTaskWoken)
{
    errorCondition = false;
    binary.giveFromISR(higherPrioTaskWoken);
}

//--------------------------------------------------------------------------------------------------
void UartAccessor::signalErrorFromIsr(BaseType_t *higherPrioTaskWoken)
{
    errorCondition = true;
    binary.giveFromISR(higherPrioTaskWoken);
}

//--------------------------------------------------------------------------------------------------
void UartAccessor::halfDuplexSwitchToRx()
{
    portENTER_CRITICAL();
    HAL_HalfDuplex_EnableReceiver(uartHandle);
    portEXIT_CRITICAL();
}

//--------------------------------------------------------------------------------------------------
void UartAccessor::halfDuplexSwitchToTx()
{
    portENTER_CRITICAL();
    HAL_HalfDuplex_EnableTransmitter(uartHandle);
    portEXIT_CRITICAL();
}