#pragma once

#include "FreeRTOS.h"
#include "i2c-drivers/bus_accessor.hpp"
#include "semphr.h"

#include "hal_header.h"

namespace i2c
{
class RtosAccessor : public IBusAccessor
{
public:
    explicit RtosAccessor(I2C_HandleTypeDef *hi2c);
    virtual bool operator==(const RtosAccessor &other) const;

    void beginTransaction(DeviceAddress address) override;
    void endTransaction() override;

    bool read(uint8_t *buffer, uint16_t length, int flags) override;
    bool write(const uint8_t *data, uint16_t length, int flags) override;

    void signalTransferCompleteFromIsr(BaseType_t *higherPrioTaskWoken = nullptr);
    void signalErrorFromIsr(BaseType_t *higherPrioTaskWoken = nullptr);

    [[nodiscard]] I2C_HandleTypeDef *getHandle() const
    {
        return i2cHandle;
    }

    static constexpr TickType_t Timeout{pdMS_TO_TICKS(100)};

private:
    static uint32_t getXferOptionsFromFlags(int flags);

    I2C_HandleTypeDef *i2cHandle;
    DeviceAddress currentAddress;
    SemaphoreHandle_t mutex = nullptr;
    SemaphoreHandle_t binary = nullptr;
    bool _errorCondition;
};
} // namespace i2c
