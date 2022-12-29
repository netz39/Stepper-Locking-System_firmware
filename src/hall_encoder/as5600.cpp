#include "as5600.hpp"
#include "hal_header.h"
#include "util/MapValue.hpp"
#include <cstdlib>

namespace AS5600
{
AS5600::AS5600(I2cAccessor &accessor, Voltage voltage, Variant variant)
    : accessor{accessor}, voltage{voltage}, deviceAddress{variant == Variant::AS5600
                                                              ? DeviceAddress
                                                              : DeviceAddressVariantL}
{
}

bool AS5600::operator==(const AS5600 &other) const
{
    return accessor == other.accessor;
}

bool AS5600::init()
{
    // sync up internal variables
    uint16_t temp;

    readWord(RegisterTwoBytes::Zpos, startPosition);
    if (commFail)
    {
        return false;
    }
    startPosition &= Zpos::ContentMask;

    readWord(RegisterTwoBytes::Mpos, stopPosition);
    if (commFail)
    {
        return false;
    }
    stopPosition &= Mpos::ContentMask;

    readWord(RegisterTwoBytes::Mang, temp);
    if (commFail)
    {
        return false;
    }
    temp &= Mang::ContentMask;
    maxAngle = static_cast<float>(temp) * Mang::ToFloat;

    readWord(RegisterTwoBytes::Conf, temp);
    if (commFail)
    {
        return false;
    }

    pwrMode = static_cast<PowerMode>((temp >> Conf::PowermodePos) & Conf::PowermodeMask);
    hystMode = static_cast<HysteresisMode>((temp >> Conf::HysteresisPos) & Conf::HysteresisMask);
    sfMode = static_cast<SlowFilterMode>((temp >> Conf::SlowfilterPos) & Conf::SlowfilterMask);
    ffth = static_cast<FastFilterThreshold>((temp >> Conf::FastFilterThresholdPos) &
                                            Conf::FastFilterThresholdMask);
    watchdog = static_cast<bool>((temp >> Conf::WatchdogPos) & Conf::WatchdogMask);

    synchronizeScaledAngle();
    initialized = true;
    return !commFail;
}

void AS5600::run()
{
    if (commFail)
    {
        reconnecting = true;
        while (commFail)
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            commFail = false;
            synchronizeScaledAngle();
        }
        // recovered communication, restore all settings again
        configureDevice(pwrMode, hystMode, sfMode, ffth, watchdog);
        if (commFail)
        {
            return; // retry everything because communication is gone again
        }

        setStartStopPosition(startPosition, stopPosition);
        if (commFail)
        {
            return;
        }

        setMaximumAngle(maxAngle);
        if (commFail)
        {
            return;
        }
        reconnecting = false;
        lastMagnetPolling = 0;
    }
    uint32_t tnow = HAL_GetTick();

    if (tnow - lastMagnetPolling > MagnetPollingTime)
    {
        lastMagnetPolling = tnow;
        // get status
        uint8_t temp;
        readByte(RegisterOneByte::Status, temp);

        if ((temp >> Status::MagnetTooStrongBit) & 0b1)
        {
            magnetStatus = -1;
        }
        else if ((temp >> Status::MagnetTooWeakBit) & 0b1)
        {
            magnetStatus = 2;
        }
        else
        {
            readByte(RegisterOneByte::Agc, temp);
            temp &= Agc::ContentMask;

            const float MinInput = 0.f;
            const float MaxInput = voltage == Voltage::ThreePointThree ? MaxValueThreePointThreeVolt
                                                                       : MaxValueFiveVolt;
            magnetStatus = util::mapValue(MinInput, MaxInput, MagnetTooCloseLimit,
                                          MagnetTooFarLimit, static_cast<float>(temp));
        }
    }

    synchronizeScaledAngle();
}

bool AS5600::configureDevice(PowerMode pwrMode, HysteresisMode hystMode, SlowFilterMode sfMode,
                             FastFilterThreshold ffth, bool watchdog)
{
    pwrMode = pwrMode;
    hystMode = hystMode;
    sfMode = sfMode;
    ffth = ffth;
    watchdog = watchdog;

    uint16_t regContent = 0;
    readWord(RegisterTwoBytes::Conf, regContent);
    if (commFail)
    {
        return false;
    }

    regContent &= Conf::SystemInternalMask << Conf::SystemInternalPos;

    // clang-format off
    regContent |= (static_cast<uint8_t>(pwrMode) & Conf::PowermodeMask)<<Conf::PowermodePos;
    regContent |= (static_cast<uint8_t>(hystMode) & Conf::HysteresisMask)<<Conf::HysteresisPos;
    // we aren't using the output pin so might aswell shut down the DAC to save power
    regContent |= (static_cast<uint8_t>(Conf::OutputStage::DigitalPWM) & Conf::OutputStageMask)<<Conf::OutputStagePos;
    // slow pwm
    regContent |= (static_cast<uint8_t>(Conf::PWMFrequency::Hz115) & Conf::PwmFreqMask)<<Conf::PwmFreqPos;
    regContent |= (static_cast<uint8_t>(sfMode) & Conf::SlowfilterMask)<<Conf::SlowfilterPos;
    regContent |= (static_cast<uint8_t>(ffth) & Conf::FastFilterThresholdMask)<<Conf::FastFilterThresholdPos;
    regContent |= (static_cast<uint8_t>(watchdog) & Conf::WatchdogMask)<<Conf::WatchdogPos;
    // clang-format on

    writeWord(RegisterTwoBytes::Conf, regContent);
    if (commFail)
    {
        return false;
    }

    synchronizeScaledAngle();
    return !commFail;
}

bool AS5600::configurePowerMode(PowerMode pwrMode)
{
    return configureDevice(pwrMode, hystMode, sfMode, ffth, watchdog);
}

void AS5600::readByte(RegisterOneByte reg, uint8_t &data)
{
    accessor.beginTransaction(deviceAddress);
    uint8_t readedByte;
    commFail = !accessor.readByteFromRegister(reg, readedByte);
    accessor.endTransaction();
}

/// Returns data in little endian order.
void AS5600::readWord(RegisterTwoBytes reg, uint16_t &data)
{
    accessor.beginTransaction(deviceAddress);
    uint16_t readedWord;
    commFail = !accessor.readWordFromRegister(reg, readedWord);
    accessor.endTransaction();
}

void AS5600::writeByte(RegisterOneByte reg, uint8_t data)
{
    accessor.beginTransaction(deviceAddress);
    commFail = !accessor.writeByteToRegister(reg, data);
    accessor.endTransaction();
}

void AS5600::writeWord(RegisterTwoBytes reg, uint16_t data)
{
    accessor.beginTransaction(deviceAddress);
    commFail = !accessor.writeWordToRegister(reg, data);
    accessor.endTransaction();
}

uint16_t AS5600::getRawAngle()
{
    uint16_t rawAngle = 0;
    readWord(RegisterTwoBytes::RawAngle, rawAngle);
    return rawAngle & RawAngle::ContentMask;
}

bool AS5600::setMaximumAngle(float maxAngle)
{
    if (maxAngle < Mang::Minimum || maxAngle > Mang::Maximum)
    {
        return false;
    }
    maxAngle = maxAngle;

    uint16_t temp = 0;
    readWord(RegisterTwoBytes::Mang, temp);
    if (commFail)
    {
        return false;
    }

    temp |= static_cast<uint16_t>(maxAngle * Mang::FromFloat) & Mang::ContentMask;
    writeWord(RegisterTwoBytes::Mang, temp);
    if (commFail)
    {
        return false;
    }

    synchronizeScaledAngle();
    return !commFail;
}

bool AS5600::setStartStopPosition(uint16_t rawStartPos, uint16_t rawStopPos)
{
    if (rawStartPos > Zpos::ContentMask || rawStopPos > Mpos::ContentMask)
    {
        return false;
    }
    uint16_t temp = 0;
    startPosition = rawStartPos;
    stopPosition = rawStopPos;

    // start
    readWord(RegisterTwoBytes::Zpos, temp);
    if (commFail)
    {
        return false;
    }

    temp &= ~Zpos::ContentMask;
    temp |= rawStartPos & Zpos::ContentMask;
    writeWord(RegisterTwoBytes::Zpos, temp);
    if (commFail)
    {
        return false;
    }

    // stop
    readWord(RegisterTwoBytes::Mpos, temp);
    if (commFail)
    {
        return false;
    }

    temp &= ~Mpos::ContentMask;
    temp |= rawStopPos & Mpos::ContentMask;
    writeWord(RegisterTwoBytes::Mpos, temp);

    synchronizeScaledAngle();
    return !commFail;
}

bool AS5600::synchronizeScaledAngle()
{
    uint16_t temp = 0;
    readWord(RegisterTwoBytes::Angle, temp);

    if (commFail)
    {
        return false;
    }

    angleScaled = temp & Angle::ContentMask;
    angleScaledRadian = static_cast<float>(angleScaled) * Angle::ToFloat;
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