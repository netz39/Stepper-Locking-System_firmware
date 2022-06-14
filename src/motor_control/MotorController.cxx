#include "MotorController.hpp"

/*
    StepControl stepControl;
    Stepper testStepper(StepperStepPin, StepperDirectionPin);
    TMC2209 tmc2209{0, &huart2};

    TMC2209::IHoldRun iHoldRun{};
    static constexpr auto TotalCurrentSteps = 31; // 4 Bit values
    auto newRegisterValue = (uint32_t)std::round(50 / 100 * TotalCurrentSteps);

    iHoldRun.iholdrun.ihold = newRegisterValue;
    iHoldRun.iholdrun.irun = newRegisterValue;
    if (!tmc2209.setIHoldRun(iHoldRun))
    {
        HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_RESET);

        __asm("bkpt");
    }
    else
    {
        HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_SET);
    }

    testStepper.setAcceleration(5000);
    testStepper.setMaxSpeed(7500);

    constexpr auto StepsPerFullRevolution = 200;
    constexpr auto MicroStepsPerStep = 8;
    constexpr auto TearDown = 5;
    constexpr auto NumberRevolutions = 2;

    constexpr auto StepsNeeded =
        NumberRevolutions * TearDown * MicroStepsPerStep * StepsPerFullRevolution;

    HAL_GPIO_WritePin(StepperEnable_GPIO_Port, StepperEnable_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_SET);

    while (true)
    {
        testStepper.setTargetRel(StepsNeeded);
        stepControl.move(testStepper);
        testStepper.setTargetRel(-StepsNeeded);
        stepControl.move(testStepper);
    }

    HAL_GPIO_WritePin(StepperEnable_GPIO_Port, StepperEnable_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_RESET);

    while (1)
    {
        // HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, GPIO_PIN_SET);

        vTaskDelay(1000);

        // HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, GPIO_PIN_RESET);

        vTaskDelay(1000);
    }
}
*/
