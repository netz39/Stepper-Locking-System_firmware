#pragma once

#include <cstdint>

namespace AS5600
{
static constexpr uint8_t DeviceAddress = 0x36;
static constexpr uint8_t DeviceAddressVariantL = 0x40;
static constexpr float Pi = 3.14159265359;

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
static constexpr uint16_t ContentMask = 0xFFF; // 12 bits
}

namespace Mpos
{
static constexpr uint16_t ContentMask = 0xFFF; // 12 bits
}

namespace Mang
{
static constexpr uint16_t ContentMask = 0xFFF; // 12 bits
static constexpr float ToFloat = (2 * Pi) / 4095.0f;
static constexpr float FromFloat = 4095.0f / (2 * Pi);

static constexpr float Minimum = 1.0f / 10.0f * Pi;
static constexpr float Maximum = 2 * Pi;
} // namespace Mang

namespace Conf
{
static constexpr uint8_t PowermodeMask = 0b11;
static constexpr uint8_t PowermodePos = 0;

static constexpr uint8_t HysteresisMask = 0b11;
static constexpr uint8_t HysteresisPos = 2;

static constexpr uint8_t OutputStageMask = 0b11;
static constexpr uint8_t OutputStagePos = 4;

// Settings for analog/pwm output pin
enum class OutputStage : uint8_t
{
    AnalogFullRange = 0,
    AnalogReducedRange,
    DigitalPWM
};

static constexpr uint8_t PwmFreqMask = 0b11;
static constexpr uint8_t PwmFreqPos = 6;

enum class PWMFrequency
{
    Hz115 = 0,
    Hz230,
    Hz460,
    Hz920
};

static constexpr uint8_t SlowfilterMask = 0b11;
static constexpr uint8_t SlowfilterPos = 8;

static constexpr uint8_t FastFilterThresholdMask = 0b111;
static constexpr uint8_t FastFilterThresholdPos = 10;

static constexpr uint8_t WatchdogMask = 0b1;
static constexpr uint8_t WatchdogPos = 13;

static constexpr uint8_t SystemInternalMask = 0b11;
static constexpr uint8_t SystemInternalPos = 14;
} // namespace Conf

namespace RawAngle
{
static constexpr uint16_t ContentMask = 0xFFF; // 12 bits
}

namespace Angle
{
static constexpr uint16_t ContentMask = 0xFFF; // 12 bits
static constexpr float ToFloat = (2 * Pi) / 4095.0f;
} // namespace Angle

namespace Status
{
static constexpr uint8_t MagnetTooStrongBit = 3;
static constexpr uint8_t MagnetTooWeakBit = 4;
static constexpr uint8_t MagnetOkayBit = 5;
} // namespace Status

namespace Agc
{
static constexpr uint8_t ContentMask = 0xFF; // 8 bits
}
} // namespace AS5600