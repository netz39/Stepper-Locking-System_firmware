#include "core/hash.hpp"
#include "core/BuildConfiguration.hpp"
#include "core/SafeAssert.h"

#if IS_EMBEDDED_BUILD()
extern char _sidata;
extern char _sdata;
extern char _edata;
#endif

namespace
{
constexpr uint32_t FlashStartAddress = 0x08000000;

uint32_t getBinarySize()
{
#if IS_EMBEDDED_BUILD() && !defined(__cppcheck__)
    return (&_sidata - reinterpret_cast<char *>(FlashStartAddress)) + (&_edata - &_sdata);
#else
    return 0;
#endif
}

uint64_t fnv(const uint8_t *data, const uint8_t *const dataEnd)
{
    return core::hash::fnvWithSeed(core::hash::HASH_SEED, data, dataEnd);
}
} // namespace

namespace core::hash
{
uint64_t computeFirmwareHash()
{
#if IS_EMBEDDED_BUILD()
    return fnv(reinterpret_cast<uint8_t *>(FlashStartAddress),
               reinterpret_cast<uint8_t *>(FlashStartAddress) + getBinarySize());
#else
    return FirmwareHashTestingValue;
#endif
}

uint64_t fnvWithSeed(uint64_t hash, const uint8_t *data, const uint8_t *const dataEnd)
{
    SafeAssert(data < dataEnd);
    SafeAssert(data != nullptr);
    SafeAssert(dataEnd != nullptr);

    for (; data < dataEnd; data++)
    {
        hash = (hash ^ *data) * MagicPrime;
    }

    return hash;
}
} // namespace core::hash