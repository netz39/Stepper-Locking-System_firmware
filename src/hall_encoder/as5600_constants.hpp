#pragma once

#include <cstdint>

namespace as5600_constants
{
constexpr uint8_t DeviceAddress = 0x36;
constexpr uint8_t DeviceAddressVariantL = 0x40;
constexpr float Pi = 3.14159265359;

enum class RegisterTwoBytes : uint8_t
{
    // Config
    Zpos = 0x01, // start position
    Mpos = 0x03, // stop position
    Mang = 0x05, // maximum angle
    Conf = 0x07,

    // Output
    RawAngle = 0x0C,
    Angle = 0x0E,

    // Status
    // MAGNITUDE = 0x1B,
};

enum class RegisterOneByte : uint8_t
{
    // Config
    // ZMCO = 0x00, // how many times zpos, mpos have been permanently written

    // Status
    Status = 0x0B,
    Agc = 0x1A,

    // Burn command not used as hardware for it is not present
};

namespace Zpos
{
constexpr uint16_t ContentMask = 0xFFF; // 12 bits
}

namespace Mpos
{
constexpr uint16_t ContentMask = 0xFFF; // 12 bits
}

namespace Mang
{
constexpr uint16_t ContentMask = 0xFFF; // 12 bits
constexpr float ToFloat = (2 * Pi) / 4095.0f;
constexpr float FromFloat = 4095.0f / (2 * Pi);

constexpr float Minimum = 1.0f / 10.0f * Pi;
constexpr float Maximum = 2 * Pi;
} // namespace Mang

namespace Conf
{
constexpr uint8_t PowermodeMask = 0b11;
constexpr uint8_t PowermodePos = 0;

constexpr uint8_t HysteresisMask = 0b11;
constexpr uint8_t HysteresisPos = 2;

constexpr uint8_t OutputStageMask = 0b11;
constexpr uint8_t OutputStagePos = 4;

// Settings for analog/pwm output pin
enum class OutputStage : uint8_t
{
    AnalogFullRange = 0,
    AnalogReducedRange,
    DigitalPWM
};

constexpr uint8_t PwmFreqMask = 0b11;
constexpr uint8_t PwmFreqPos = 6;

enum class PWMFrequency
{
    Hz115 = 0,
    Hz230,
    Hz460,
    Hz920
};

constexpr uint8_t SlowfilterMask = 0b11;
constexpr uint8_t SlowfilterPos = 8;

constexpr uint8_t FastFilterThresholdMask = 0b111;
constexpr uint8_t FastFilterThresholdPos = 10;

constexpr uint8_t WatchdogMask = 0b1;
constexpr uint8_t WatchdogPos = 13;

constexpr uint8_t SystemInternalMask = 0b11;
constexpr uint8_t SystemInternalPos = 14;
} // namespace Conf

namespace RawAngle
{
constexpr uint16_t ContentMask = 0xFFF; // 12 bits
}

namespace Angle
{
constexpr uint16_t ContentMask = 0xFFF; // 12 bits
constexpr float ToFloat = (2 * Pi) / 4095.0f;
} // namespace Angle

namespace Status
{
constexpr uint8_t MagnetTooStrongBit = 3;
constexpr uint8_t MagnetTooWeakBit = 4;
constexpr uint8_t MagnetOkayBit = 5;
} // namespace Status

namespace Agc
{
constexpr uint8_t ContentMask = 0xFF; // 8 bits
}
} // namespace as5600_constants