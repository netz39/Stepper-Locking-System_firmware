#pragma once

#include "adc.h"

#include "units/si/current.hpp"
#include "units/si/frequency.hpp"
#include "units/si/resistance.hpp"
#include "units/si/scalar.hpp"
#include "units/si/temperature.hpp"
#include "units/si/voltage.hpp"

#include "wrappers/Task.hpp"

using namespace units::si;
using util::wrappers::TaskWithMemberFunctionBase;

class AnalogDigital : public TaskWithMemberFunctionBase
{
public:
    AnalogDigital() : TaskWithMemberFunctionBase("adcTask", 1024, osPriorityLow6){};

    static constexpr auto AdcPeripherie = &hadc1;
    static constexpr auto TotalChannelCount = 4;
    static constexpr auto SampleCount = 16;

    static constexpr auto VrefRank = 0;
    static constexpr auto MotorTemperatureRank = 1;
    static constexpr auto InputCurrentRank = 2;
    static constexpr auto McuTemperatureRank = 3;

    static constexpr auto CurrentMeasurementFactor = 1.0_A / 2.0_V;

    Voltage referenceVoltage{};
    Current inputCurrent{}; // on 12V rail

    Temperature motorTemperature{};
    Temperature mcuTemperature{};

    void conversionCompleteCallback();

protected:
    void taskMain() override;

private:
    static constexpr auto NtcBetaValue = 3380.0f;
    static constexpr auto NtcNominalTemperature = 25.0_degC;
    static constexpr auto NtcResistanceAtNominalTemperature = 10_kOhm;
    static constexpr auto NtcSecondResistor = 10_kOhm;

    using AdcResultType = uint16_t;
    std::array<AdcResultType, TotalChannelCount> adcResults{};

    void waitUntilConversionFinished();
    void calibrateAdc();
    void startConversion();
    void calculateReferenceVoltage();

    Temperature calculateNtcTemperature(const Voltage dropVoltage);

    template <class T>
    constexpr auto physicalQuantityFromAdcResult(const AdcResultType adcResult, const T multiplier)
    {
        constexpr auto AdcResolution = 12;
        constexpr auto MaxAdcValue = (1 << AdcResolution) - 1;

        return (adcResult * referenceVoltage * multiplier) / MaxAdcValue;
    }

    template <class T>
    void updateFastLowpass(T &oldValue, const T newSample, const uint8_t sampleCount)
    {
        oldValue += (newSample - oldValue) / static_cast<float>(sampleCount);
    }
};