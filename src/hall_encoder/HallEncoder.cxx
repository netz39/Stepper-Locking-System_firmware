#include "HallEncoder.hpp"
#include "motor_control/MotorController.hpp"
#include "sync.hpp"

#include "helpers/freertos.hpp"
#include "units/si/frequency.hpp"

//--------------------------------------------------------------------------------------------------
void HallEncoder::taskMain()
{
    // wait for EEPROM
    sync::waitForAll(sync::ConfigurationLoaded);

    configureHall();

    // init and set prev value with current position
    device.run();
    prevHallEncoderRawValue = getRawPosition();

    auto lastWakeTime = xTaskGetTickCount();
    while (true)
    {
        device.run();

        if (isEncoderOkay())
            calculatePosition();

        constexpr auto TaskFrequency = 200.0_Hz;
        vTaskDelayUntil(&lastWakeTime, toOsTicks(TaskFrequency));
    }
}

//--------------------------------------------------------------------------------------------------
void HallEncoder::configureHall()
{
    device.configureDevice(
        AS5600::AS5600::PowerMode::LPM1,      // LPM1 -> polling time 5 ms - current = ~3.4mA
        AS5600::AS5600::HysteresisMode::LSB1, // one LSB filter
        AS5600::AS5600::SlowFilterMode::x16,  // default value
        AS5600::AS5600::FastFilterThreshold::slowFilter, // default value
        true                                             // watchdog, power saving after one minute
    );
}

//--------------------------------------------------------------------------------------------------
void HallEncoder::onSettingsUpdate()
{
    // the hall encoder is mounted at the front of the stepper,
    // so its rotation direction is inverted in comparison to the motor direction
    // due the downwards mounting of the sensor PCB

    // hall encoder is incrementing when the magnet rotates clockwise
    isIncrementingAtOpening =
        !settingsContainer.getValue<firmwareSettings::InvertRotationDirection>();
}

//--------------------------------------------------------------------------------------------------
void HallEncoder::calculatePosition()
{
    const uint16_t CurrentPosition = getRawPosition();
    int32_t diff = isIncrementingAtOpening ? (CurrentPosition - prevHallEncoderRawValue)
                                           : (prevHallEncoderRawValue - CurrentPosition);

    prevHallEncoderRawValue = CurrentPosition;

    if (diff == 0)
        return;

    // cross over detection - 360° to 0° and vice versa
    if (diff < -EncoderResolution / 2)
        diff += EncoderResolution;

    else if (diff > EncoderResolution / 2)
        diff -= EncoderResolution;

    constexpr auto EncoderToMicrostepsFactor =
        (float)EncoderResolution / MotorController::MicrostepsPerRevolution;

    const float CoveredMicrosteps = (float)diff / EncoderToMicrostepsFactor;

    accumulatedPosition += CoveredMicrosteps;
}
//--------------------------------------------------------------------------------------------------
bool HallEncoder::isEncoderOkay()
{
    return device.isOK();
}

//--------------------------------------------------------------------------------------------------
int32_t HallEncoder::getPosition()
{
    return std::round(accumulatedPosition);
}

//--------------------------------------------------------------------------------------------------
uint16_t HallEncoder::getRawPosition()
{
    // start and stop pos are not set, so we can use the "scaled" angle like raw angle
    return device.getAngleScaled();
}

//--------------------------------------------------------------------------------------------------
void HallEncoder::setPosition(int32_t microsteps)
{
    accumulatedPosition = microsteps;
    prevHallEncoderRawValue = getRawPosition();
}