#include "i2c-drivers/rtos_accessor.hpp"

namespace i2c
{
RtosAccessor::RtosAccessor(I2C_HandleTypeDef *hi2c) : i2cHandle{hi2c}
{
    mutex = xSemaphoreCreateMutex();
    binary = xSemaphoreCreateBinary();
}

bool RtosAccessor::operator==(const RtosAccessor &other) const
{
    return i2cHandle == other.i2cHandle;
}

void RtosAccessor::beginTransaction(i2c::DeviceAddress address)
{
    xSemaphoreTake(mutex, portMAX_DELAY);
    currentAddress = address;
}

void RtosAccessor::endTransaction()
{
    xSemaphoreGive(mutex);
}

bool RtosAccessor::read(uint8_t *buffer, uint16_t length, int flags)
{
    _errorCondition = false;
    HAL_I2C_Master_Seq_Receive_IT(i2cHandle, currentAddress << 1, buffer, length,
                                  getXferOptionsFromFlags(flags));

    // Casestudy AS5600L encoder
    // somehow it is possible to get the chip into an invalid internal state which causes it
    // to not communicate anymore and pull down the SDA line permanently
    // STM chips doesn't seem to fire any interrupt when this happens so a semaphore timeout is
    // necessary
    const auto semphrSuccess = xSemaphoreTake(binary, Timeout);
    if (semphrSuccess == pdFALSE)
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
    HAL_I2C_Master_Seq_Transmit_IT(i2cHandle, currentAddress << 1, const_cast<uint8_t *>(data),
                                   length, getXferOptionsFromFlags(flags));

    // Casestudy AS5600L encoder
    // somehow it is possible to get the chip into an invalid internal state which causes it
    // to not communicate anymore and pull down the SDA line permanently
    // STM chips doesn't seem to fire any interrupt when this happens so a semaphore timeout is
    // necessary
    const auto semphrSuccess = xSemaphoreTake(binary, Timeout);
    if (semphrSuccess == pdFALSE)
    {
        _errorCondition = true;
    }

    bool errCnd = _errorCondition;
    _errorCondition = false;
    return !errCnd;
}

uint32_t RtosAccessor::getXferOptionsFromFlags(int flags)
{
    uint32_t options = 0;

    if (flags & i2c::FirstFrame)
    {
        options = I2C_FIRST_FRAME;
    }
    else if (flags & i2c::LastFrame)
    {
        options = I2C_LAST_FRAME;
    }
    else if ((flags & i2c::FirstFrame) && (flags & i2c::LastFrame))
    {
        options = I2C_FIRST_AND_LAST_FRAME;
    }
    else
    {
        options = I2C_NEXT_FRAME;
    }

    return options;
}

void RtosAccessor::signalTransferCompleteFromIsr(BaseType_t *higherPrioTaskWoken)
{
    _errorCondition = false;
    xSemaphoreGiveFromISR(binary, higherPrioTaskWoken);
}

void RtosAccessor::signalErrorFromIsr(BaseType_t *higherPrioTaskWoken)
{
    _errorCondition = true;
    xSemaphoreGiveFromISR(binary, higherPrioTaskWoken);
}
} // namespace i2c