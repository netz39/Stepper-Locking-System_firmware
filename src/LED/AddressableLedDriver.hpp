#pragma once

#include "GammaCorrection.hpp"
#include "LedDataTypes.hpp"
#include "core/SafeAssert.h"
#include "units/si/time.hpp"

#include "stm32g0xx_hal.h"

// Controls the addressable LEDs over SPI hardware
class AddressableLedDriver
{
public:
    static constexpr auto Timeout = 2.0_s;

    explicit AddressableLedDriver(SPI_HandleTypeDef *spiPeripherie) : spiPeripherie(spiPeripherie)
    {
        SafeAssert(spiPeripherie != nullptr);
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
            color.blue = GammaCorrectionLUT[newColor.blue];
            color.green = GammaCorrectionLUT[newColor.green];
            color.red = GammaCorrectionLUT[newColor.red];
        }
    };
    using LedSpiDataArray = std::array<LedSpiData, NumberOfLedsPerRing>;

    LedSpiDataArray ledSpiData1;
    LedSpiDataArray ledSpiData2;

    static constexpr auto NumberOfEndFrames = (NumberOfRings * NumberOfLedsPerRing + 15) / 16;
    std::array<uint8_t, NumberOfEndFrames> endFrames{};

    void sendStartFrame();

    /// convert LED data to gamma corrected colors and put it to SPI-related array
    void convertToGammaCorrectedColors(LedSegmentArray &source, LedSpiDataArray &destination);
};