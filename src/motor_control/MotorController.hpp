#pragma once

#include "main.h"
#include "usart.h"

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
    static constexpr auto DebugUartPeripherie = &huart1;
    static constexpr auto TmcUartPeripherie = &huart2;

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
    /// @param forceInvert invert the moving direction
    void doCalibration(bool forceInvert = false);

    /// Stops calibration movement and restore motor params
    void abortCalibration();

    /// Stops calibration movement, restore motor params and save position
    void calibrationIsDone();

    // called by TeensyStep when it is finished with a movement
    void finishedCallback();

    /// Interrupt opening action and return to position before opening -> fully closed
    void revertOpening();

    /// Interrupt closing action and return to position before closing -> fully opened
    void revertClosing();

    using Callback = std::function<void()>;

    /// set up callback which will be called when the target is reached
    void setFinishedCallback(Callback newCallback)
    {
        callback = newCallback;
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
    bool ignoreFinishedEvent = false;

    StepControl stepControl{};
    Stepper stepperMotor{StepperStepPin, StepperDirectionPin};
    TMC2209 tmc2209{0, TmcUartPeripherie};
    util::Gpio stepperEnable{StepperEnable_GPIO_Port, StepperEnable_Pin};

    Callback callback = nullptr;

    uint8_t maximumMotorCurrentInPercentage = 0;
    uint32_t maximumMotorSpeed = 0;
    uint32_t maximumMotorAcc = 0;
    uint32_t calibrationSpeed = 0;

    /// Moves the motor asynchronously.
    /// @param microSteps moves the motor the given microSteps.
    /// Postive values closes the door.
    void moveRelative(int32_t microSteps);

    /// Moves the motor asynchronously.
    /// @param position moves the motor to given position.
    void moveAbsolute(int32_t position);

    /// Stops a currently active movement with deaccerlation ramp.
    /// Blocking.
    void stopMovement();

    /// Stops a currently active movement immediately.
    void stopMovementImmediately();

    /// Enables Calibration mode:
    /// - reduces max speed
    /// - sets motor current to 100%
    void enableCalibrationMode();

    /// Disables calibration mode:
    /// - restores the settings (speed/motor current)
    void disableCalibrationMode();

    /// Setter for maximum motor current in percent
    void setMotorMaxCurrentPercentage(uint8_t percentage);

    void enableMotorTorque();

    void disableMotorTorque();

    void checkMotorTemperature();
};