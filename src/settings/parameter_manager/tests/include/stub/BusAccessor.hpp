#pragma once
#include <i2c-drivers/bus_accessor.hpp>

class BusAccessorStub : public i2c::IBusAccessor
{
public:
    virtual void beginTransaction(i2c::DeviceAddress address) override{};
    virtual void endTransaction() override{};

    virtual bool read(uint8_t *buffer, uint16_t length, int flags) override
    {
        return true;
    };
    virtual bool write(const uint8_t *data, uint16_t length, int flags) override
    {
        return true;
    };
};
