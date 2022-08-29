#include "i2c-drivers/rtos_accessor.hpp"
#include <core/SafeAssert.h>

namespace i2c
{
RtosAccessor::RtosAccessor(I2C_HandleTypeDef *hi2c) : i2cHandle{hi2c}
{
}

bool RtosAccessor::operator==(const RtosAccessor &other) const
{
    return i2cHandle == other.i2cHandle;
}

void RtosAccessor::beginTransaction(i2c::DeviceAddress address)
{
    mutex.lock();
    currentAddress = address;
}

void RtosAccessor::endTransaction()
{
    mutex.unlock();
}

bool RtosAccessor::read(uint8_t *buffer, uint16_t length, int flags)
{
    _errorCondition = false;
    {
        portENTER_CRITICAL();
        const auto Ret = HAL_I2C_Master_Seq_Receive_IT(i2cHandle, currentAddress << 1, buffer,
                                                       length, getXferOptionsFromFlags(flags));
        SafeAssert(Ret == HAL_OK);
        portEXIT_CRITICAL();
    }

    if (binary.take(Timeout) == pdFALSE)
    {
        _errorCondition = true;
    }

    bool errCnd = _errorCondition;
    _errorCondition = false;
    return !errCnd;
}

bool RtosAccessor::write(const uint8_t *data, uint16_t length, int flags)
{
    _errorCondition = false;
    {
        portENTER_CRITICAL();
        const auto Ret = HAL_I2C_Master_Seq_Transmit_IT(i2cHandle, currentAddress << 1,
                                                        const_cast<uint8_t *>(data), length,
                                                        getXferOptionsFromFlags(flags));
        SafeAssert(Ret == HAL_OK);
        portEXIT_CRITICAL();
    }

    if (binary.take(Timeout) == pdFALSE)
    {
        _errorCondition = true;
    }

    bool errCnd = _errorCondition;
    _errorCondition = false;
    return !errCnd;
}

void RtosAccessor::signalTransferCompleteFromIsr()
{
    auto higherPrioTaskWoken = pdFALSE;
    _errorCondition = false;
    binary.giveFromISR(&higherPrioTaskWoken);
    portYIELD_FROM_ISR(higherPrioTaskWoken);
}

void RtosAccessor::signalErrorFromIsr()
{
    auto higherPrioTaskWoken = pdFALSE;
    _errorCondition = true;
    binary.giveFromISR(&higherPrioTaskWoken);
    portYIELD_FROM_ISR(higherPrioTaskWoken);
}
} // namespace i2c