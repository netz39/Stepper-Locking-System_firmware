#include "i2c-drivers/as5600.hpp"
#include "hal_header.h"
#include "util/MapValue.hpp"
#include <cstdlib>

namespace AS5600
{
AS5600::AS5600(i2c::RtosAccessor &accessor, Voltage voltage, Variant variant, bool createTask,
               const char *taskName, UBaseType_t prio)
    : accessor_{accessor}, voltage_{voltage}, deviceAddress_{variant == Variant::AS5600
                                                                 ? DEVICE_ADDRESS
                                                                 : DEVICE_ADDRESS_L_VARIANT}
{
    if (createTask)
    {
        xTaskCreate(&AS5600::taskEntry, taskName, 200, this, prio, &task_);
    }
}

AS5600::~AS5600()
{
    if (task_ != nullptr)
    {
#if INCLUDE_vTaskDelete
        vTaskDelete(task_);
#else
        std::abort();
#endif
    }
}

bool AS5600::operator==(const AS5600 &other) const
{
    return accessor_ == other.accessor_;
}

bool AS5600::init()
{
    // sync up internal variables
    uint16_t temp;

    rwTwoBytes(Direction::READ, RegisterTwoBytes::ZPOS, &startPosition_);
    if (commFail_)
    {
        return false;
    }
    startPosition_ &= Zpos::CONTENT_MASK;

    rwTwoBytes(Direction::READ, RegisterTwoBytes::MPOS, &stopPosition_);
    if (commFail_)
    {
        return false;
    }
    stopPosition_ &= Mpos::CONTENT_MASK;

    rwTwoBytes(Direction::READ, RegisterTwoBytes::MANG, &temp);
    if (commFail_)
    {
        return false;
    }
    temp &= Mang::CONTENT_MASK;
    maxAngle_ = static_cast<float>(temp) * Mang::toFloat;

    rwTwoBytes(Direction::READ, RegisterTwoBytes::CONF, &temp);
    if (commFail_)
    {
        return false;
    }

    pwrMode_ = static_cast<PowerMode>((temp >> Conf::POWERMODE_POS) & Conf::POWERMODE_MASK);
    hystMode_ = static_cast<HysteresisMode>((temp >> Conf::HYSTERESIS_POS) & Conf::HYSTERESIS_MASK);
    sfMode_ = static_cast<SlowFilterMode>((temp >> Conf::SLOWFILTER_POS) & Conf::SLOWFILTER_MASK);
    ffth_ = static_cast<FastFilterThreshold>((temp >> Conf::FAST_FILTER_THRESHOLD_POS) &
                                             Conf::FAST_FILTER_THRESHOLD_MASK);
    watchdog_ = static_cast<bool>((temp >> Conf::WATCHDOG_POS) & Conf::WATCHDOG_MASK);

    synchronizeScaledAngle();
    initialized_ = true;
    return !commFail_;
}

[[noreturn]] void AS5600::taskEntry(void *context)
{
    while (!static_cast<AS5600 *>(context)->init())
        ;

    for (;;)
    {
        static_cast<AS5600 *>(context)->run(true);
    }
}

void AS5600::swapBytes(uint16_t &val)
{
    val = ((val & 0xFF) << 8) | (val >> 8);
}

void AS5600::run(bool blocking)
{
    if (commFail_)
    {
        reconnecting_ = true;
        while (commFail_)
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            commFail_ = false;
            synchronizeScaledAngle();
        }
        // recovered communication, restore all settings again
        configureDevice(pwrMode_, hystMode_, sfMode_, ffth_, watchdog_);
        if (commFail_)
        {
            return; // retry everything because communication is gone again
        }

        setStartStopPosition(startPosition_, stopPosition_);
        if (commFail_)
        {
            return;
        }

        setMaximumAngle(maxAngle_);
        if (commFail_)
        {
            return;
        }
        reconnecting_ = false;
        lastAnglePolling_ = 0;
        lastMagnetPolling_ = 0;
    }
    uint32_t tnow = HAL_GetTick();

    if (tnow - lastMagnetPolling_ > MagnetPollingTime)
    {
        lastMagnetPolling_ = tnow;
        // get status
        uint8_t temp;
        rwOneByte(Direction::READ, RegisterOneByte::STATUS, &temp);

        if ((temp >> Status::MAGNET_TOO_STRONG_POS) & Status::MAGNET_TOO_STRONG_MASK)
        {
            magnetStatus_ = -1;
        }
        else if ((temp >> Status::MAGNET_TOO_WEAK_POS) & Status::MAGNET_TOO_WEAK_MASK)
        {
            magnetStatus_ = 2;
        }
        else
        {
            rwOneByte(Direction::READ, RegisterOneByte::AGC, &temp);
            temp &= AGC::CONTENT_MASK;

            const float minInput = 0.f;
            const float maxInput = voltage_ == Voltage::ThreePointThree ? MaxValueThreePointThreeVolt
                                                                        : MaxValueFiveVolt;
            magnetStatus_ =
                util::mapValue(minInput, maxInput, MagnetTooCloseLimit, MagnetTooFarLimit, static_cast<float>(temp));
        }
    }

    if (tnow - lastAnglePolling_ > getPollingTime(pwrMode_))
    {
        synchronizeScaledAngle();
        lastAnglePolling_ = HAL_GetTick();
    }
    else if (blocking)
    {
        vTaskDelay(getPollingTime(pwrMode_) - (tnow - lastAnglePolling_));
        synchronizeScaledAngle();
        lastAnglePolling_ = HAL_GetTick();
    }
}

bool AS5600::configureDevice(PowerMode pwrMode, HysteresisMode hystMode, SlowFilterMode sfMode,
                             FastFilterThreshold ffth, bool watchdog)
{
    pwrMode_ = pwrMode;
    hystMode_ = hystMode;
    sfMode_ = sfMode;
    ffth_ = ffth;
    watchdog_ = watchdog;

    uint16_t regContent = 0;
    rwTwoBytes(Direction::READ, RegisterTwoBytes::CONF, &regContent);
    if (commFail_)
    {
        return false;
    }

    regContent &= Conf::SYSTEM_INTERNAL_MASK << Conf::SYSTEM_INTERNAL_POS;

    // clang-format off
    regContent |= (static_cast<uint8_t>(pwrMode) & Conf::POWERMODE_MASK)<<Conf::POWERMODE_POS;
    regContent |= (static_cast<uint8_t>(hystMode) & Conf::HYSTERESIS_MASK)<<Conf::HYSTERESIS_POS;
    // we aren't using the output pin so might aswell shut down the DAC to save power
    regContent |= (static_cast<uint8_t>(Conf::OutputStage::digitalPWM) & Conf::OUTPUT_STAGE_MASK)<<Conf::OUTPUT_STAGE_POS;
    // slow pwm
    regContent |= (static_cast<uint8_t>(Conf::PWMFrequency::Hz115) & Conf::PWM_FREQ_MASK)<<Conf::PWM_FREQ_POS;
    regContent |= (static_cast<uint8_t>(sfMode) & Conf::SLOWFILTER_MASK)<<Conf::SLOWFILTER_POS;
    regContent |= (static_cast<uint8_t>(ffth) & Conf::FAST_FILTER_THRESHOLD_MASK)<<Conf::FAST_FILTER_THRESHOLD_POS;
    regContent |= (static_cast<uint8_t>(watchdog) & Conf::WATCHDOG_MASK)<<Conf::WATCHDOG_POS;
    // clang-format on

    rwTwoBytes(Direction::WRITE, RegisterTwoBytes::CONF, &regContent);
    if (commFail_)
    {
        return false;
    }

    synchronizeScaledAngle();
    return !commFail_;
}

bool AS5600::configurePowerMode(PowerMode pwrMode)
{
    return configureDevice(pwrMode, hystMode_, sfMode_, ffth_, watchdog_);
}

void AS5600::rwTwoBytes(Direction dir, RegisterTwoBytes reg, uint16_t *data)
{
    auto regAddr = static_cast<uint8_t>(reg);
    accessor_.beginTransaction(deviceAddress_);
    commFail_ = !accessor_.write(reinterpret_cast<const uint8_t *>(&regAddr), 1, i2c::FirstFrame);

    switch (dir)
    {
        case Direction::READ:
            commFail_ = !accessor_.read(reinterpret_cast<uint8_t *>(data), 2, i2c::LastFrame);
            swapBytes(*data);
            break;
        case Direction::WRITE:
            swapBytes(*data);
            commFail_ =
                !accessor_.write(reinterpret_cast<const uint8_t *>(data), 2, i2c::LastFrame);
            swapBytes(*data);
            break;
        default:
            std::abort();
    }
    accessor_.endTransaction();
}

void AS5600::rwOneByte(Direction dir, RegisterOneByte reg, uint8_t *data)
{
    auto regAddr = static_cast<uint8_t>(reg);
    accessor_.beginTransaction(deviceAddress_);
    commFail_ = !accessor_.write(reinterpret_cast<const uint8_t *>(&regAddr), 1, i2c::FirstFrame);

    switch (dir)
    {
        case Direction::READ:
            commFail_ = !accessor_.read(data, 1, i2c::LastFrame);
            break;
        case Direction::WRITE:
            commFail_ = !accessor_.write(data, 1, i2c::LastFrame);
            break;
        default:
            std::abort();
    }
    accessor_.endTransaction();
}

uint16_t AS5600::getRawAngle()
{
    uint16_t rawAngle = 0;
    rwTwoBytes(Direction::READ, RegisterTwoBytes::RAW_ANGLE, &rawAngle);
    return rawAngle & RawAngle::CONTENT_MASK;
}

bool AS5600::setMaximumAngle(float maxAngle)
{
    if (maxAngle < Mang::minimum || maxAngle > Mang::maximum)
    {
        return false;
    }
    maxAngle_ = maxAngle;

    uint16_t temp = 0;
    rwTwoBytes(Direction::READ, RegisterTwoBytes::MANG, &temp);
    if (commFail_)
    {
        return false;
    }

    temp |= static_cast<uint16_t>(maxAngle * Mang::fromFloat) & Mang::CONTENT_MASK;
    rwTwoBytes(Direction::WRITE, RegisterTwoBytes::MANG, &temp);
    if (commFail_)
    {
        return false;
    }

    synchronizeScaledAngle();
    return !commFail_;
}

bool AS5600::setStartStopPosition(uint16_t rawStartPos, uint16_t rawStopPos)
{
    if (rawStartPos > Zpos::CONTENT_MASK || rawStopPos > Mpos::CONTENT_MASK)
    {
        return false;
    }
    uint16_t temp = 0;
    startPosition_ = rawStartPos;
    stopPosition_ = rawStopPos;

    // start
    rwTwoBytes(Direction::READ, RegisterTwoBytes::ZPOS, &temp);
    if (commFail_)
    {
        return false;
    }

    temp &= ~Zpos::CONTENT_MASK;
    temp |= rawStartPos & Zpos::CONTENT_MASK;
    rwTwoBytes(Direction::WRITE, RegisterTwoBytes::ZPOS, &temp);
    if (commFail_)
    {
        return false;
    }

    // stop
    rwTwoBytes(Direction::READ, RegisterTwoBytes::MPOS, &temp);
    if (commFail_)
    {
        return false;
    }

    temp &= ~Mpos::CONTENT_MASK;
    temp |= rawStopPos & Mpos::CONTENT_MASK;
    rwTwoBytes(Direction::WRITE, RegisterTwoBytes::MPOS, &temp);

    synchronizeScaledAngle();
    return !commFail_;
}

bool AS5600::synchronizeScaledAngle()
{
    uint16_t temp = 0;
    rwTwoBytes(Direction::READ, RegisterTwoBytes::ANGLE, &temp);
    if (commFail_)
    {
        return false;
    }
    angleScaled_ = temp & Angle::CONTENT_MASK;
    angleScaledRadian_ = static_cast<float>(angleScaled_) * Angle::toFloat;
    return true;
}

TickType_t AS5600::getPollingTime(PowerMode pwrMode)
{
    switch (pwrMode)
    {
        case PowerMode::Normal:
            return pdMS_TO_TICKS(2);
        case PowerMode::LPM1:
            return pdMS_TO_TICKS(5);
        case PowerMode::LPM2:
            return pdMS_TO_TICKS(20);
        case PowerMode::LPM3:
            return pdMS_TO_TICKS(100);
        default:
            std::abort();
    }
}
} // namespace AS5600