#pragma once

#include <cstdint>

namespace core::hash
{
    static constexpr uint64_t HASH_SEED = 0xcbf29ce484222325;
    static constexpr uint64_t MagicPrime = 0x00000100000001b3;

    uint64_t fnvWithSeed(uint64_t hash, const uint8_t *data, const uint8_t *const dataEnd);

    static constexpr uint64_t FirmwareHashTestingValue{0xDEADBEEFC0FFEE};
    uint64_t computeFirmwareHash();
} // namespace bus_node_base
