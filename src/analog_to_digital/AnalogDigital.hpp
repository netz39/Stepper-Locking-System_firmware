#pragma once

#include "adc.h" // todo handles should be inserted via constructor to make class testable

#include "units/si/current.hpp"
#include "units/si/frequency.hpp"
#include "units/si/resistance.hpp"
#include "units/si/scalar.hpp"
#include "units/si/temperature.hpp"
#include "units/si/voltage.hpp"

#include "wrappers/Task.hpp"

/// Convert analog values into digital.
/// Measurement of input current, motor temperature and µC temperature
class AnalogDigital : public util::wrappers::TaskWithMemberFunctionBase
{
public:
    AnalogDigital()
        : TaskWithMemberFunctionBase("adcTask", 1024, osPriorityLow6){};

    static constexpr auto AdcPeripherie = &hadc1; // todo handles should be inserted via constructor to make class testable
    static constexpr auto TotalChannelCount = 4;
    static constexpr auto SampleCount = 16;

    static constexpr auto VrefRank = 3;
    static constexpr auto MotorTemperatureRank = 0;
    static constexpr auto InputCurrentRank = 1;
    static constexpr auto McuTemperatureRank = 2;

    static constexpr auto CurrentMeasurementFactor = 1.0_A / 2.0_V;


    void conversionCompleteCallback();

    [[nodiscard]] units::si::Temperature getMotorTemperature() const noexcept
    {
        return motorTemperature;
    }
    [[nodiscard]] units::si::Temperature getMCUTemperature() const noexcept
    {
        return mcuTemperature;
    }

protected:
    [[noreturn]] void taskMain() override;

private:
    static constexpr auto NtcBetaValue = 3380.0f;
    static constexpr auto NtcNominalTemperature = 25.0_degC;
    static constexpr auto NtcResistanceAtNominalTemperature = 10_kOhm;
    static constexpr auto NtcSecondResistor = 10_kOhm;


    units::si::Voltage referenceVoltage{};
    units::si::Current inputCurrent{}; // on 12V rail
    units::si::Temperature motorTemperature{};
    units::si::Temperature mcuTemperature{};

    using AdcResultType = uint16_t;
    std::array<AdcResultType, TotalChannelCount> adcResults{};

    void waitUntilConversionFinished();
    void calibrateAdc();
    void startConversion();
    void calculateReferenceVoltage();

    units::si::Temperature calculateNtcTemperature(units::si::Voltage dropVoltage);

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