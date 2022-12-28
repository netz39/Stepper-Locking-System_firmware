#pragma once

#include <cstdint>
#include <type_traits>

namespace i2c
{
using DeviceAddress = uint16_t;

enum TransferFlags : int
{
    NoFlags = 0,
    FirstFrame = 1 << 0,
    LastFrame = 1 << 1,
    NextFrame = 1 << 2
};

class IBusAccessor
{
public:
    virtual ~IBusAccessor() = default;
    virtual void beginTransaction(DeviceAddress address) = 0;
    virtual void endTransaction() = 0;

    virtual bool read(uint8_t *buffer, uint16_t length, int flags) = 0;
    virtual bool write(const uint8_t *data, uint16_t length, int flags) = 0;
};
} // namespace i2c
