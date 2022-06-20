# i2c-drivers
Collection of I²C drivers. Requires bus-node-base inside project to work.

### AS 5600 Encoder
*May be used on the same bus with 24LCxx, just use the same bus accessor*

I²C Host Settings:
 - Fase Mode: Enabled
 - Clock speed: 400kBd
 - Fast Mode Duty Cycle: Tlow/Thigh = 2
 - Clock No Stretch Mode: Disabled
 - Own Address: 0
 - Primary Address Length: 7 Bit
 - Dual Address: Disabled
 - General Call Address Detection: Disabled

CubeMX: 
 - Enable I2C Error, Event Interrupt

Makefile Integration:

- Add i2c-drivers/include to include path
- Add i2c-drivers/src/rtos_accessor.cpp and i2c-drivers/src/as5600.cpp to sources list


### 24LCxx EEPROM

I²C Host Settings: 
 - Fase Mode: Enabled
 - Clock speed: 400kBd
 - Fast Mode Duty Cycle: Tlow/Thigh = 2
 - Clock No Stretch Mode: Disabled
 - Own Address: 0
 - Primary Address Length: 7 Bit
 - Dual Address: Disabled
 - General Call Address Detection: Disabled

CubeMX: 
 - Enable I2C Error, Event Interrupt

Makefile Integration:

- Add i2c-drivers/include to include path
- Add i2c-drivers/src/rtos_accessor.cpp to sources list


Code Integration:

```c++
//Example
#include "main.h"
#include "i2c-drivers/24lcxx.hpp"
#include "i2c-drivers/rtos_accessor.hpp"

// handle for i2c peripheral
extern I2C_HandleTypeDef hi2c3;

namespace {
    i2c::RtosAccessor _i2cBusAccessor(hi2c3);
    Eeprom24LC64 _eeprom(_i2cBusAccessor, 0b000 /* Three Address pins on chip */);
}

void TaskMethod() {
    uint32_t value = 0xDEADBEEF;
    uint32_t value2 = 0;
    ::_eeprom.write(0 /* Address*/ , (uint8_t *)&value, sizeof(value));
    ::_eeprom.read(0 /* Address*/ , (uint8_t *)&value2, sizeof(value2));

    for(;;)
    {
        vTaksDelay(pdMS_TO_TICKS(1000));
    }
}

void _signalFromISR(bool error)
{
    auto higherPrioTaskWoken = pdFALSE;
    if (error) {
        ::_i2cBusAccessor.signalErrorFromIsr(&higherPrioTaskWoken);
    } else {
        ::_i2cBusAccessor.signalTransferCompleteFromIsr(&higherPrioTaskWoken);
    }
    portYIELD_FROM_ISR(higherPrioTaskWoken);
}

// Either directly use weak defined CubeHAL callbacks or register them 
// This example is using weak callbacks

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    _signalFromISR(false);
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    _signalFromISR(false);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
    _signalFromISR(true);
}
```
