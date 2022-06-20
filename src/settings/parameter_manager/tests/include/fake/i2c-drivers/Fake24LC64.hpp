#pragma once
#include <cstdint>
#include <cstring>
#include <i2c-drivers/24lcxx.hpp>

class FakeEeprom24LC64 : public Eeprom24LC64
{
public:
    explicit FakeEeprom24LC64(i2c::IBusAccessor& accessor) : Eeprom24LC64(accessor, 0),
          fakeMemory{0}
    {
        fakeMemory.fill(0xFF);
    }

    void read(Address_t address, uint8_t *buffer, Address_t length) override
    {
        SafeAssert(length != 0);
        SafeAssert(length - 1 <= std::numeric_limits<uint16_t>::max());
        SafeAssert(!doesAddressExceedLimit(address + length - 1));

        std::memcpy(buffer, fakeMemory.data() + address, length);
    }

    void write(Address_t address, const uint8_t *data, Address_t length) override
    {
        SafeAssert(length != 0);
        SafeAssert(length - 1 <= std::numeric_limits<uint16_t>::max());
        SafeAssert(!doesAddressExceedLimit(address + length - 1));

        std::memcpy(fakeMemory.data() + address, data, length);
    }

private:
    std::array<uint8_t, getSizeInBytes()> fakeMemory;
};