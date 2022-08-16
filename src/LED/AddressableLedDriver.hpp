#pragma once

#include "LedDataTypes.hpp"
#include "core/SafeAssert.h"

#include "stm32g0xx_hal.h"

// Controls the addressable LEDs over SPI hardware
class AddressableLedDriver
{
public:
    static constexpr auto TimeoutInMilliseconds = 2000;

    explicit AddressableLedDriver(SPI_HandleTypeDef *spiPeripherie) : spiPeripherie(spiPeripherie)
    {
        SafeAssert(spiPeripherie != nullptr);
        endFrames.fill(0xFF);
    };

    void sendBuffer(LedSegmentArray &firstArray, LedSegmentArray &secondArray);

private:
    SPI_HandleTypeDef *spiPeripherie = nullptr;

    LedSpiDataArray ledSpiData1;
    LedSpiDataArray ledSpiData2;

    static constexpr auto NumberOfEndFrames = (NumberOfRings * NumberOfLedsPerRing + 15) / 16;
    std::array<uint8_t, NumberOfEndFrames> endFrames{};

    void sendStartFrame();

    /// convert LED data to gamma corrected colors and put it to SPI-related array
    void convertToGammaCorrectedColors(LedSegmentArray &source, LedSpiDataArray &destination);
};