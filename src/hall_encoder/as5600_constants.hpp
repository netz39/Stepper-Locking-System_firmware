#pragma once

#include "FreeRTOS.h"

namespace AS5600
{
static constexpr uint8_t DEVICE_ADDRESS = 0x36;
static constexpr uint8_t DEVICE_ADDRESS_L_VARIANT = 0x40;
static constexpr float PI = 3.14159265359;

enum class RegisterTwoBytes : uint8_t
{
    /* Config */
    ZPOS = 0x01, // start position
    MPOS = 0x03, // stop position
    MANG = 0x05, // maximum angle
    CONF = 0x07,

    /* Output */
    RAW_ANGLE = 0x0C,
    ANGLE = 0x0E,

    /* Status */
    // MAGNITUDE = 0x1B,
};

enum class RegisterOneByte : uint8_t
{
    /* Config */
    // ZMCO = 0x00, // how many times zpos, mpos have been permanently written

    /* Status */
    STATUS = 0x0B,
    AGC = 0x1A,

    /* Burn command not used as hardware for it is not present */
};

namespace Zpos
{
static constexpr uint16_t CONTENT_MASK = 4095; // 12 bits
}
namespace Mpos
{
static constexpr uint16_t CONTENT_MASK = 4095; // 12 bits
}
namespace Mang
{
static constexpr uint16_t CONTENT_MASK = 4095; // 12 bits
static constexpr float toFloat = (2 * PI) / 4095.0f;
static constexpr float fromFloat = 4095.0f / (2 * PI);

static constexpr float minimum = 1.0f / 10.0f * PI;
static constexpr float maximum = 2 * PI;
} // namespace Mang

namespace Conf
{
static constexpr uint8_t POWERMODE_MASK = 0b11;
static constexpr uint8_t POWERMODE_POS = 0;

static constexpr uint8_t HYSTERESIS_MASK = 0b11;
static constexpr uint8_t HYSTERESIS_POS = 2;

static constexpr uint8_t OUTPUT_STAGE_MASK = 0b11;
static constexpr uint8_t OUTPUT_STAGE_POS = 4;

// Settings for analog / pwm output pin
enum class OutputStage : uint8_t
{
    analogFullRange = 0,
    analogReducedRange,
    digitalPWM
};

static constexpr uint8_t PWM_FREQ_MASK = 0b11;
static constexpr uint8_t PWM_FREQ_POS = 6;
enum class PWMFrequency
{
    Hz115 = 0,
    Hz230,
    Hz460,
    Hz920
};

static constexpr uint8_t SLOWFILTER_MASK = 0b11;
static constexpr uint8_t SLOWFILTER_POS = 8;

static constexpr uint8_t FAST_FILTER_THRESHOLD_MASK = 0b111;
static constexpr uint8_t FAST_FILTER_THRESHOLD_POS = 10;

static constexpr uint8_t WATCHDOG_MASK = 0b1;
static constexpr uint8_t WATCHDOG_POS = 13;

static constexpr uint8_t SYSTEM_INTERNAL_MASK = 0b11;
static constexpr uint8_t SYSTEM_INTERNAL_POS = 14;
} // namespace Conf

namespace RawAngle
{
static constexpr uint16_t CONTENT_MASK = 4095; // 12 bits
}

namespace Angle
{
static constexpr uint16_t CONTENT_MASK = 4095; // 12 bits
static constexpr float toFloat = (2 * PI) / 4095.0f;
} // namespace Angle

namespace Status
{
static constexpr uint8_t MAGNET_TOO_STRONG_MASK = 0b1;
static constexpr uint8_t MAGNET_TOO_STRONG_POS = 3;

static constexpr uint8_t MAGNET_TOO_WEAK_MASK = 0b1;
static constexpr uint8_t MAGNET_TOO_WEAK_POS = 4;

static constexpr uint8_t MAGNET_OK_MASK = 0b1;
static constexpr uint8_t MAGNET_OK_POS = 5;
} // namespace Status

namespace AGC
{
static constexpr uint8_t CONTENT_MASK = 255; // 8 bits
}
} // namespace AS5600