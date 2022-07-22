#include "HallEncoder.hpp"
#include "motor_control/MotorController.hpp"
#include "sync.hpp"

#include "helpers/freertos.hpp"
#include "units/si/frequency.hpp"

//--------------------------------------------------------------------------------------------------
[[noreturn]] void HallEncoder::taskMain()
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
        constexpr auto TaskFrequency = 200.0_Hz;
        vTaskDelayUntil(&lastWakeTime, toOsTicks(TaskFrequency));

        device.run();

        if (!isOkay())
            continue;

        detectCrossovers();
        calculatePosition();
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
        !settingsContainer.getValue<firmwareSettings::InvertRotationDirection, bool>();
}

//--------------------------------------------------------------------------------------------------
void HallEncoder::calculatePosition()
{
    const uint16_t CurrentEncoderRawValue = getRawPosition();

    const int32_t Diff = isIncrementingAtOpening
                             ? (CurrentEncoderRawValue - encoderValueAtHomePoint)
                             : (encoderValueAtHomePoint - CurrentEncoderRawValue);

    currentPosition = Diff * MotorController::MicrostepsPerRevolution / EncoderResolution;
    currentPosition += currentTurn * MotorController::MicrostepsPerRevolution;
}

//--------------------------------------------------------------------------------------------------
void HallEncoder::detectCrossovers()
{
    const auto CurrentEncoderRawValue = getRawPosition();
    const int32_t Diff = CurrentEncoderRawValue - prevHallEncoderRawValue;

    if (Diff == 0)
        return;

    if (Diff > EncoderResolution / 2)
    {
        currentTurn = currentTurn + (isIncrementingAtOpening ? -1 : 1);
    }
    else if (Diff < -EncoderResolution / 2)
    {
        currentTurn = currentTurn + (isIncrementingAtOpening ? 1 : -1);
    }

    prevHallEncoderRawValue = CurrentEncoderRawValue;
}

//--------------------------------------------------------------------------------------------------
bool HallEncoder::isOkay()
{
    return device.isOK();
}

//--------------------------------------------------------------------------------------------------
int32_t HallEncoder::getPosition()
{
    return currentPosition;
}

//--------------------------------------------------------------------------------------------------
uint16_t HallEncoder::getRawPosition()
{
    // start and stop pos are not set, so we can use the "scaled" angle like raw angle
    return device.getAngleScaled();
}

//--------------------------------------------------------------------------------------------------
bool HallEncoder::saveHomePosition()
{
    if (!isOkay())
        return false;

    encoderValueAtHomePoint = getRawPosition();
    currentTurn = 0;
    return true;
}