#pragma once

#include "main.h"

#include "helpers/freertos.hpp"
#include "parameter_manager/SettingsUser.hpp"
#include "settings/Settings.hpp"
#include "units/si/temperature.hpp"
#include "util/gpio.hpp"
#include "wrappers/Task.hpp"

#include "TMC2209.hpp"
#include "TeensyStep.h"

using units::si::Temperature;
using util::wrappers::TaskWithMemberFunctionBase;

/// This class is used to control the stepper motor.
/// It contains a wrapper of the needed functions of the stepper library as well as additional
/// functions to monitor the stepper´s state.
class MotorController : public TaskWithMemberFunctionBase, SettingsUser
{
public:
    static constexpr auto UartPeripherie = &huart2;

    MotorController(firmwareSettings::Container &settingsContainer, Temperature &motorTemperature)
        : TaskWithMemberFunctionBase("motorControllerTask", 128, osPriorityAboveNormal3), //
          settingsContainer(settingsContainer),                                           //
          motorTemperature(motorTemperature)                                              //

    {
        stepControl.setCallback(std::bind(&MotorController::finishedCallback, this));
    }

    void openDoor();

    void closeDoor();

    /// calibration by homing the lock switch
    void doCalibration();

    [[nodiscard]] bool isCalibrating() const;

    void calibrationIsDone();

    // called by TeensyStep when it is finished with a movement
    void finishedCallback();

    [[nodiscard]] bool isRunning() const
    {
        return stepControl.isRunning();
    }

protected:
    void taskMain() override;

    void onSettingsUpdate() override;

private:
    static constexpr auto MicrostepsPerFullStep = 8;
    static constexpr auto NumberOfFullSteps = 200;
    static constexpr auto MicrostepsPerRevolution = MicrostepsPerFullStep * NumberOfFullSteps;
    static constexpr auto GearReduction = 5;
    static constexpr auto NeededRevolutions = 1.75;
    static constexpr int32_t NumberOfMicrostepsForOperation =
        NeededRevolutions * MicrostepsPerRevolution * GearReduction;

    static constexpr auto CalibrationSpeed = 2000;
    static constexpr auto CalibrationAcc = 3000;

    static constexpr auto WarningMotorTemp = 70.0_degC;
    static constexpr auto CriticalMotorTemp = 85.0_degC;

    firmwareSettings::Container &settingsContainer;

    Temperature &motorTemperature;
    bool isOverheated = false;
    uint32_t overheatedCounter;
    bool hasWarningTemp = false;
    uint32_t warningTempCounter;

    bool isInCalibrationMode = false;
    bool isDirectionInverted = false;

    StepControl stepControl{};
    Stepper stepperMotor{StepperStepPin, StepperDirectionPin};
    TMC2209 tmc2209{0, UartPeripherie};
    util::Gpio stepperEnable{StepperEnable_GPIO_Port, StepperEnable_Pin};

    uint8_t maximumMotorCurrentInPercentage = 0;
    uint32_t maximumMotorSpeed = 0;
    uint32_t maximumMotorAcc = 0;

    /// Moves the motor asynchronously.
    /// @param microSteps moves the motor the given microSteps.
    /// Postive values closes the door.
    void moveRelative(int32_t microSteps);

    /// Stops a currently active movement with deaccerlation ramp.
    /// Blocking.
    void stopMovement();

    /// Stops a currently active movement immediately.
    void stopMovementImmediately();

    /// Enables Calibration mode:
    /// - stops a current movement
    /// - reduces max speed / acceleration
    /// - sets motor current to 100%
    void enableCalibrationMode();

    /// Disables calibration mode:
    /// - stops a current movement
    /// - restores the settings (speed/acceleration/motor current)
    void disableCalibrationMode();

    /// Setter for maximum motor current in percent
    void setMotorMaxCurrentPercentage(uint8_t percentage);

    void enableMotorTorque();

    void disableMotorTorque();

    void checkMotorTemperature();
};