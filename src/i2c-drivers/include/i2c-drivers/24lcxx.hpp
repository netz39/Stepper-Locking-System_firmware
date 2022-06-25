#pragma once
#include "FreeRTOS.h"
#include "task.h"

#include "i2c-drivers/bus_accessor.hpp"
#include <array>
#include <limits>
#include <core/SafeAssert.h>

template <size_t KiB, size_t BytesPerPage>
class Eeprom24LCXX
{
public:
    using Address_t = size_t;
    Eeprom24LCXX(i2c::IBusAccessor &accessor, i2c::DeviceAddress chipSelectBits)
        : accessor{accessor}, deviceAddress{static_cast<i2c::DeviceAddress>(
                                  BaseAddress | (chipSelectBits & ChipSelectMask))}
    {
        static_assert(std::numeric_limits<Address_t>::min() == 0,
                      "Choose Address_t to be unsigned!");

        static_assert(
            static_cast<uint64_t>(getSizeInBytes()-1) <=
                static_cast<uint64_t>(std::numeric_limits<Address_t>::max()),
            "Address_t is not able to cover complete size of eeprom. Choose a bigger type");

        static_assert(static_cast<uint64_t>(getSizeInBytes()-1) <=
                          static_cast<uint64_t>(std::numeric_limits<uint16_t>::max()),
                      "read() and write() are still statically written for 2-byte addresses. "
                      "Revise implementations");
    }

    static constexpr size_t getSizeInBytes()
    {
        return KiB * 1024;
    }

    virtual void read(Address_t address, uint8_t *buffer, Address_t length)
    {
        SafeAssert(length != 0);
        SafeAssert(length - 1<= std::numeric_limits<uint16_t>::max());
        SafeAssert(!doesAddressExceedLimit(address + length - 1));

        auto memoryAddressBuffer = getAddressBuffer(static_cast<uint16_t>(address));

        accessor.beginTransaction(deviceAddress);
        accessor.write(memoryAddressBuffer.data(), 2, i2c::FirstFrame);
        accessor.read(buffer, length, i2c::LastFrame);
        accessor.endTransaction();
    }

    virtual void write(Address_t address, const uint8_t *data, Address_t length)
    {
        SafeAssert(length != 0);
        SafeAssert(length - 1 <= std::numeric_limits<uint16_t>::max());
        SafeAssert(!doesAddressExceedLimit(address + length - 1));

        const auto firstPage = address / BytesPerPage;
        const auto lastPage = (address + length) / BytesPerPage;
        const auto numberOfFullPages = firstPage == lastPage ? 0 : lastPage - firstPage - 1;

        const auto numberOfBytesOnFirstPage =
            std::min(BytesPerPage - (address % BytesPerPage), length);
        const auto numberOfBytesOnLastPage = (address + length) % BytesPerPage;

        accessor.beginTransaction(deviceAddress);

        // Write first page
        auto memoryAddressBuffer = getAddressBuffer(address);
        accessor.write(memoryAddressBuffer.data(), 2, i2c::FirstFrame);
        accessor.write(data, numberOfBytesOnFirstPage, i2c::LastFrame);
        waitPageWriteFinished();

        address += numberOfBytesOnFirstPage;
        data += numberOfBytesOnFirstPage;

        for (size_t i = 0; i < numberOfFullPages; ++i)
        {
            memoryAddressBuffer = getAddressBuffer(address);
            accessor.write(memoryAddressBuffer.data(), 2, i2c::FirstFrame);
            accessor.write(data, BytesPerPage, i2c::LastFrame);
            waitPageWriteFinished();

            address += BytesPerPage;
            data += BytesPerPage;
        }

        // Write last page
        if (firstPage != lastPage && numberOfBytesOnLastPage > 0)
        {
            memoryAddressBuffer = getAddressBuffer(address);
            accessor.write(memoryAddressBuffer.data(), 2, i2c::FirstFrame);
            accessor.write(data, numberOfBytesOnLastPage, i2c::LastFrame);
            waitPageWriteFinished();
        }

        accessor.endTransaction();
    }

protected:
    using MemoryAddressBuffer = std::array<uint8_t, 2>;

    static bool doesAddressExceedLimit(Address_t address)
    {
        return address >= getSizeInBytes();
    }

    static MemoryAddressBuffer getAddressBuffer(uint16_t address)
    {
        MemoryAddressBuffer buffer{static_cast<uint8_t>((address & 0xff00) >> 8),
                                   static_cast<uint8_t>(address & 0xff)};
        return buffer;
    }

    void waitPageWriteFinished()
    {
        // Do ACK Polling
        while (!accessor.write(nullptr, 0, i2c::LastFrame))
        {
            // chip takes about 5ms to store the data
            // no use in polling constantly and eating up cpu
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }

    i2c::IBusAccessor &accessor;
    i2c::DeviceAddress deviceAddress;

    static constexpr auto BaseAddress = 0b1010 << 3;
    static constexpr auto ChipSelectMask = 0b111;
};

using Eeprom24LC64 = Eeprom24LCXX<64, 32>;