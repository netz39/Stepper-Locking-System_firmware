#pragma once
#include "i2c-drivers/bus_accessor.hpp"
#include <FreeRTOS.h>
#include <hal_header.h>
#include <wrappers/BinarySemaphore.hpp>
#include <wrappers/Mutex.hpp>

namespace i2c
{
class RtosAccessor : public IBusAccessor
{
public:
    explicit RtosAccessor(I2C_HandleTypeDef *hi2c);
    ~RtosAccessor() override = default;
    virtual bool operator==(const RtosAccessor &other) const;

    void beginTransaction(DeviceAddress address) override;
    void endTransaction() override;

    bool read(uint8_t *buffer, uint16_t length, int flags) override;
    bool write(const uint8_t *data, uint16_t length, int flags) override;

    void signalTransferCompleteFromIsr();
    void signalErrorFromIsr();

    [[nodiscard]] I2C_HandleTypeDef *getHandle() const
    {
        return i2cHandle;
    }

    // Case-study AS5600L encoder
    // somehow it is possible to get the chip into an invalid internal state which causes it
    // to not communicate anymore and pull down the SDA line permanently
    // STM chips doesn't seem to fire any interrupt when this happens so a semaphore timeout is
    // necessary
    static constexpr TickType_t Timeout{pdMS_TO_TICKS(100)};

private:
    I2C_HandleTypeDef *i2cHandle;
    DeviceAddress currentAddress{};
    util::wrappers::Mutex mutex;
    util::wrappers::BinarySemaphore binary;
    volatile bool _errorCondition{false};

    static constexpr uint32_t getXferOptionsFromFlags(const int flags)
    {
        if ((flags & i2c::FirstFrame) && (flags & i2c::LastFrame))
            return I2C_FIRST_AND_LAST_FRAME;

        else if ((flags & i2c::FirstFrame) && (flags & i2c::NextFrame))
            return I2C_FIRST_AND_NEXT_FRAME;

        else if (flags & i2c::FirstFrame)
            return I2C_FIRST_FRAME;

        else if (flags & i2c::LastFrame)
            return I2C_LAST_FRAME;

        else
            return I2C_NEXT_FRAME;
    }
};
} // namespace i2c
