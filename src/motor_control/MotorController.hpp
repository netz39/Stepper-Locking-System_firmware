#pragma once

#include "main.h"
#include "usart.h"

#include "hall_encoder/HallEncoder.hpp"
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

    enum class FailureType
    {
        None,
        ExcessiveStepLosses,
        CalibrationFailed,
        MotorMovedExternally
    };

    MotorController(firmwareSettings::Container &settingsContainer, Temperature &motorTemperature,
                    HallEncoder &hallEncoder)
        : TaskWithMemberFunctionBase("motorControllerTask", 128, osPriorityAboveNormal3), //
          settingsContainer(settingsContainer),                                           //
          motorTemperature(motorTemperature),                                             //
          hallEncoder(hallEncoder)                                                        //
    {
        stepControl.setCallback(std::bind(&MotorController::invokeFinishedCallback, this));
    }

    void openDoor();

    void closeDoor();

    /// Start a movement which is slow
    /// for manual drive
    void manualOpen();

    /// Start a movement which is slow
    /// for manual drive
    void manualClose();

    /// calibration by homing the lock switch
    /// moves the stepper in closing direction
    /// @param forceInvert invert the moving direction - true will "opening" the door
    void doCalibration(bool forceInvert = false);

    /// Stops calibration movement and restore motor params
    void abortCalibration();

    /// Stops calibration movement, restore motor params and save position
    void calibrationIsDone();

    // called by TeensyStep when it is finished with a movement
    void invokeFinishedCallback();

    /// Interrupt opening action and return to position before opening -> fully closed
    void revertOpening();

    /// Interrupt closing action and return to position before closing -> fully opened
    void revertClosing();

    /// Permit further motor movements.
    void freezeMotor();

    /// Allow further motor movements again.
    void unfreezeMotor();

    /// Stops a currently active movement with deaccerlation ramp.
    /// Blocking.
    void stopMovement();

    /// Stops a currently active movement immediately.
    void stopMovementImmediately();

    /// should be called after using manualOpen or manualClose to stop the movement and reset the
    /// motor settings
    void disableManualMode();

    /// set state to not calibrated
    void revokeCalibration();

    /// Return the progess of opening/closing actions in percentage.
    uint8_t getProgress();

    using Callback = std::function<void(FailureType failureType)>;

    /// set up callback which will be called when the target is reached
    void setFinishedCallback(Callback newCallback)
    {
        finishedCallback = newCallback;
    }

    static constexpr auto MicrostepsPerFullStep = 8;
    static constexpr auto NumberOfFullSteps = 200;
    static constexpr auto MicrostepsPerRevolution = MicrostepsPerFullStep * NumberOfFullSteps;
    static constexpr auto GearReduction = 5;
    static constexpr auto NeededRevolutions = 1.75;
    static constexpr int32_t NumberOfMicrostepsForOperation =
        NeededRevolutions * MicrostepsPerRevolution * GearReduction;

    static constexpr auto MicrostepLossThreshold = 64;
    static constexpr auto StepLossEventCounterThreshold = 256;
    static constexpr auto StepLossEventAtCalibrationCounterThreshold = 16;

    static constexpr auto WarningMotorTemp = 70.0_degC;
    static constexpr auto CriticalMotorTemp = 85.0_degC;

protected:
    void taskMain() override;

    void onSettingsUpdate() override;

private:
    firmwareSettings::Container &settingsContainer;
    Temperature &motorTemperature;
    HallEncoder &hallEncoder;

    bool isOverheated = false;
    uint32_t overheatedCounter;
    bool hasWarningTemp = false;
    uint32_t warningTempCounter;

    bool isInCalibrationMode = false;
    bool isCalibrating = false;
    bool isCalibrated = false;
    bool ignoreFinishedEvent = false;
    bool isMotorFreezed = false;

    // only for progess bar
    bool isOpening = false;
    bool isClosing = false;

    StepControl stepControl{};
    Stepper stepperMotor{StepperStepPin, StepperDirectionPin};
    TMC2209 tmc2209{0, TmcUartPeripherie};
    util::Gpio stepperEnable{StepperEnable_GPIO_Port, StepperEnable_Pin};

    Callback finishedCallback = nullptr;

    uint8_t maximumMotorCurrentInPercentage = 0;
    uint32_t maximumMotorSpeed = 0;
    uint32_t maximumMotorAcc = 0;
    uint32_t calibrationSpeed = 0;
    uint32_t calibrationAcc = 0;

    uint32_t stepLossEventCounter = 0;

    /// Moves the motor asynchronously.
    /// @param microSteps moves the motor the given microSteps.
    /// Postive values opens the door.
    void moveRelative(int32_t microSteps);

    /// Moves the motor asynchronously.
    /// @param position moves the motor to given position.
    void moveAbsolute(int32_t position);

    /// - reduces max speed/acc
    /// - sets motor current to 100%
    void applyCalibrationMotorSettings();

    /// Set motor settings to values saved in EEPROM
    void applyNormalMotorSettings();

    /// Enables Calibration mode:
    /// - update motor settings
    void enableCalibrationMode();

    /// Disables calibration mode:
    /// - restores motor settings
    void disableCalibrationMode();

    /// Setter for maximum motor current in percent
    void setMotorMaxCurrentPercentage(uint8_t percentage);

    /// enable power to motor
    void enableMotorTorque();

    /// disable power to motor - no heating
    void disableMotorTorque();

    void checkMotorTemperature();

    void signalSuccess();

    void signalFailure(FailureType failureType);

    bool checkForStepLosses();
};