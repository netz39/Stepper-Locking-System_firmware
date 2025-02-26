#pragma once

#include "FreeRTOS.h"
#include "LedDataTypes.hpp"
#include "spi.h"
#include "units/si/time.hpp"
#include "util/led/GammaCorrection.hpp"

// Controls the addressable LEDs over SPI hardware
class AddressableLedDriver
{
public:
    static constexpr auto Timeout = 2.0_s;

    explicit AddressableLedDriver(SPI_HandleTypeDef *spiPeripherie) : spiPeripherie(spiPeripherie)
    {
        configASSERT(spiPeripherie != nullptr);
        endFrames.fill(0xFF);
    };

    void sendBuffer(LedSegmentArray &firstArray, LedSegmentArray &secondArray);

private:
    SPI_HandleTypeDef *spiPeripherie = nullptr;

    struct LedSpiData
    {
        uint8_t Start = 0xFF; //!< the first byte contains control data like brightness
        BgrColor color;

        void assignGammaCorrectedColor(BgrColor newColor)
        {
            color.blue = GammaCorrection.LookUpTable[newColor.blue];
            color.green = GammaCorrection.LookUpTable[newColor.green];
            color.red = GammaCorrection.LookUpTable[newColor.red];
        }
    };
    using LedSpiDataArray = std::array<LedSpiData, NumberOfLedsPerRing>;

    LedSpiDataArray ledSpiData1;
    LedSpiDataArray ledSpiData2;

    static constexpr auto NumberOfEndFrames = (NumberOfRings * NumberOfLedsPerRing + 15) / 16;
    std::array<uint8_t, NumberOfEndFrames> endFrames{};

    static constexpr size_t PwmSteps = 256;
    static constexpr auto ResolutionBits = std::bit_width<size_t>(PwmSteps - 1);
    using GammaCorrection_t = util::led::pwm::GammaCorrection<ResolutionBits, 1.0f>;
    static constexpr GammaCorrection_t GammaCorrection{};

    void sendStartFrame();

    /// convert LED data to gamma corrected colors and put it to SPI-related array
    void convertToGammaCorrectedColors(LedSegmentArray &source, LedSpiDataArray &destination);
};