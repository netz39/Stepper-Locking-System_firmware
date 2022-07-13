#include "UartAccessor.hpp"
#include "helpers/freertos.hpp"

void UartAccessor::beginTransaction()
{
    xSemaphoreTake(mutex, portMAX_DELAY);
}

//--------------------------------------------------------------------------------------------------
void UartAccessor::endTransaction()
{
    xSemaphoreGive(mutex);
}

//--------------------------------------------------------------------------------------------------
bool UartAccessor::receive(uint8_t *buffer, uint16_t length)
{
    errorCondition = false;

    HAL_UART_Receive_DMA(uartHandle, buffer, length);
    errorCondition = (xSemaphoreTake(binary, toOsTicks(Timeout)) == pdFALSE);

    bool returnValue = errorCondition;
    errorCondition = false;
    return !returnValue;
}

//--------------------------------------------------------------------------------------------------
bool UartAccessor::transmit(const uint8_t *data, uint16_t length)
{
    errorCondition = false;

    HAL_UART_Transmit_DMA(uartHandle, const_cast<uint8_t *>(data), length);
    errorCondition = (xSemaphoreTake(binary, toOsTicks(Timeout)) == pdFALSE);

    bool returnValue = errorCondition;
    errorCondition = false;
    return !returnValue;
}

//--------------------------------------------------------------------------------------------------
void UartAccessor::signalTransferCompleteFromIsr(BaseType_t *higherPrioTaskWoken)
{
    errorCondition = false;
    xSemaphoreGiveFromISR(binary, higherPrioTaskWoken);
}

//--------------------------------------------------------------------------------------------------
void UartAccessor::signalErrorFromIsr(BaseType_t *higherPrioTaskWoken)
{
    errorCondition = true;
    xSemaphoreGiveFromISR(binary, higherPrioTaskWoken);
}

//--------------------------------------------------------------------------------------------------
void UartAccessor::halfDuplexSwitchToRx()
{
    HAL_HalfDuplex_EnableReceiver(uartHandle);
}

//--------------------------------------------------------------------------------------------------
void UartAccessor::halfDuplexSwitchToTx()
{
    HAL_HalfDuplex_EnableTransmitter(uartHandle);
}