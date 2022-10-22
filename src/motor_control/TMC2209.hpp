#pragma once

#include "usart.h"
#include <cstdint>
#include <optional>

#include "UartAccessor.hpp"

// Class to control the Trinamic IC, contains register accesses etc
class TMC2209
{
private:
    uint8_t slaveaddress;
    bool commOk = true;

    UartAccessor &uartAccessor;

    struct ReadRequest
    {
        uint32_t syncAddr : 8;
        uint32_t slaveAddr : 8;
        uint32_t regAddr : 7;
        uint32_t rw : 1;
        uint32_t crc : 8;
    } __attribute__((packed));

    struct DataReverse
    {
        uint8_t data1;
        uint8_t data2;
        uint8_t data3;
        uint8_t data4;
    };

    union DataReverseUnion
    {
        DataReverse dataReverse;
        uint32_t data;
    };

    struct ReadReply
    {
        uint32_t syncAddr : 8;
        uint32_t slaveAddr : 8;
        uint32_t regAddr : 7;
        uint32_t rw : 1;
        uint32_t data1 : 8;
        uint32_t data2 : 8;
        uint32_t data3 : 8;
        uint32_t data4 : 8;
        uint32_t crc : 8;
    } __attribute__((packed));

    union ReadReqReply
    {
        ReadReply readReply;
        uint64_t readreqreply;
    };

    struct WriteAccess
    {
        uint32_t syncAddr : 8;
        uint32_t slaveAddr : 8;
        uint32_t regAddr : 7;
        uint32_t rw : 1;
        uint32_t data : 32;
        uint32_t crc : 8;
    } __attribute__((packed));

    struct OTPProgPins
    {
        uint32_t otpBit : 3;
        uint32_t reserved1 : 1;
        uint32_t otpByte : 2;
        uint32_t reserved2 : 2;
        uint32_t otpMagic : 8;
        uint32_t reserved : 16;
    } __attribute__((packed));

    union OTPProg
    {
        OTPProgPins otpProgPins;
        uint32_t otpProg : 32;
    };

    struct OTPReadPins
    {
        uint32_t fclkTrim : 5;
        uint32_t ottrim : 1;
        uint32_t intRSense : 1;
        uint32_t tbl : 1;
        uint32_t pwgradOrChopconf3 : 4;
        uint32_t pwautogradOrChopconf4 : 1;
        uint32_t pwthresOrChopconf5 : 3;
        uint32_t pwofsOrChopconf8 : 1;
        uint32_t pwreg : 1;
        uint32_t pwfreq : 1;
        uint32_t iholdDelay : 2;
        uint32_t ihold : 2;
        uint32_t enSpreadCycle : 1;
        uint32_t reserved : 8;
    } __attribute__((packed));

    union OTPRead
    {
        OTPReadPins otpReadPins;
        uint32_t otpRead : 32;
    };

    struct IFCountPins
    {
        uint32_t count : 8;
        uint32_t reserved : 24;
    } __attribute__((packed));

    union IFCount
    {
        IFCountPins ifCountPins;
        uint32_t ifCount : 32;
    };

    struct WriteDataBits
    {
        uint32_t data1 : 8;
        uint32_t data2 : 8;
        uint32_t data3 : 8;
        uint32_t data4 : 8;
    } __attribute__((packed));

    union WriteData
    {
        WriteDataBits writeDataBits;
        uint32_t data : 32;
    };

public:
    TMC2209(uint8_t slave, UartAccessor &uartAccessor);
    TMC2209(TMC2209 const &other) = default;
    TMC2209(TMC2209 &&other) = delete;
    TMC2209 &operator=(TMC2209 const &other) = delete;
    TMC2209 &operator=(TMC2209 &&other) = delete;
    ~TMC2209() = default;

    static constexpr auto DrvStatus = 0x6F;
    static constexpr auto Gconf = 0x00;
    static constexpr auto Gstat = 0x01;
    static constexpr auto Ifcnt = 0x02;
    static constexpr auto Slaveconf = 0x03;
    static constexpr auto Ioin = 0x06;
    static constexpr auto OtpRead = 0x05;
    static constexpr auto OtpProgram = 0x04;
    static constexpr auto FactoryConf = 0x07;
    static constexpr auto IholdIrun = 0x10;
    static constexpr auto TpowerDown = 0x11;
    static constexpr auto TstepRegister = 0x12;
    static constexpr auto TpwmThrs = 0x13;
    static constexpr auto Vactual = 0x22;
    static constexpr auto TcoolThrs = 0x14;
    static constexpr auto SgThrs = 0x40;
    static constexpr auto SgResult = 0x41;
    static constexpr auto CoolConf = 0x42;
    static constexpr auto MsCnt = 0x6A;
    static constexpr auto MscurAct = 0x6B;
    static constexpr auto ChopConf = 0x6C;
    static constexpr auto PwmConf = 0x70;
    static constexpr auto PwScale = 0x71;
    static constexpr auto PwAuto = 0x72;

    struct SlaveConfigPins
    {
        uint32_t reserved : 8;
        uint32_t lsb0 : 1;
        uint32_t lsb1 : 1;
        uint32_t msb0 : 1;
        uint32_t msb1 : 1;
        uint32_t reserved2 : 20;
    } __attribute__((packed));

    union SlaveConfig
    {
        SlaveConfigPins slaveConfigPins;
        uint32_t slaveConfig : 32;
    };

    struct GStatsPins
    {
        uint32_t reset : 1;
        uint32_t drvErr : 1;
        uint32_t uvCp : 1;
        uint32_t reserved : 29;
    } __attribute__((packed));

    union GStats
    {
        GStatsPins gStatsPins;
        uint32_t gStatus : 32;
    };

    struct CoolConfigurationPins
    {
        uint32_t semin : 4;
        uint32_t reserved : 1;
        uint32_t seup : 2;
        uint32_t reserved1 : 1;
        uint32_t semax : 4;
        uint32_t reserved2 : 1;
        uint32_t sedn : 2;
        uint32_t seimin : 1;
        uint32_t reserved_3 : 16;
    } __attribute__((packed));

    union CoolConfiguration
    {
        CoolConfigurationPins coolConfPins;
        uint32_t coolConfValue : 32;
    };

    struct DrvStatsRegister
    {
        uint32_t overtemperaturePreWarning : 1;
        uint32_t overtemperatureFlag : 1;
        uint32_t shortToGndA : 1;
        uint32_t shortToGndB : 1;
        uint32_t shortLsA : 1;
        uint32_t shortLsB : 1;
        uint32_t openLoadA : 1;
        uint32_t openLoadB : 1;
        uint32_t tempExceeded120 : 1;
        uint32_t tempExceeded143 : 1;
        uint32_t tempExceeded150 : 1;
        uint32_t tempExceeded157 : 1;
        uint32_t reserved : 4;
        uint32_t csActual : 5;
        uint32_t reserved1 : 9;
        uint32_t stealthChopIndicator : 1;
        uint32_t standstillIndicator : 1;
    } __attribute__((packed));

    union DrvStats
    {
        DrvStatsRegister drvStatsRegister;
        uint32_t drvStatus : 32;
    };

    struct InputConfigPins
    {
        uint32_t enn : 1;
        uint32_t gnd : 1;
        uint32_t ms_1 : 1;
        uint32_t ms_2 : 1;
        uint32_t diag : 1;
        uint32_t ground : 1;
        uint32_t pdn_uart : 1;
        uint32_t step : 1;
        uint32_t spread_en : 1;
        uint32_t dir : 1;
        uint32_t reserved : 14;
    } __attribute__((packed));

    union InputConfig
    {
        InputConfigPins inputConfigPins;
        uint32_t inputConf : 32;
    };

    struct PWMConfPins
    {
        uint32_t pwofs : 8;
        uint32_t pwgrad : 8;
        uint32_t pwfreq : 2;
        uint32_t pwauto_scale : 1;
        uint32_t pwauto_grad : 1;
        uint32_t freewheel : 2;
        uint32_t reserved : 2;
        uint32_t pwreg : 4;
        uint32_t pwlim : 4;
    } __attribute__((packed));

    union PWMConfig
    {
        PWMConfPins pwmConfPins;
        uint32_t pwmConfValue : 32;
    };

    struct ChopperConfPins
    {
        uint32_t toff : 4;
        uint32_t hstrt : 3;
        uint32_t hend : 4;
        uint32_t reserved : 4;
        uint32_t tbl : 2;
        uint32_t vsense : 1;
        uint32_t reserved1 : 6;
        uint32_t mres : 4;
        uint32_t intpol : 1;
        uint32_t dedge : 1;
        uint32_t diss2g : 1;
        uint32_t diss2vs : 1;
    } __attribute__((packed));

    union ChopperConfig
    {
        ChopperConfPins chopperConfPins;
        uint32_t chopperConfValue : 32;
    };

    struct FactoryConfigurationPins
    {
        uint32_t fclkTrim : 5;
        uint32_t reserved : 3;
        uint32_t otTrim : 2;
        uint32_t reserved1 : 22;
    } __attribute__((packed));

    union FactoryConfigurations
    {
        FactoryConfigurationPins factoryConfigurationPins;
        uint32_t factConf : 32;
    };

    struct PWMScalePins
    {
        uint32_t pwscaleSum : 8;
        uint32_t reserved : 8;
        uint32_t pwscaleAuto : 9;
        uint32_t reserved1 : 7;
    } __attribute__((packed));

    union PWMScale
    {
        PWMScalePins pwmScalePins;
        uint32_t pwscaleValue : 32;
    };

    struct PWMAutoPins
    {
        uint32_t pwofsAuto : 8;
        uint32_t reserved : 8;
        uint32_t pwgradAuto : 8;
        uint32_t reserved1 : 8;
    } __attribute__((packed));

    union PWMAuto
    {
        PWMScalePins pwmAutoPins;
        uint32_t pwautoValue : 32;
    };

    struct TPowerDownPins
    {
        uint32_t tPowerDown : 8;
        uint32_t reserved : 24;
    } __attribute__((packed));

    union TPowerDown
    {
        TPowerDownPins tPowerDownPins;
        uint32_t tPowerDownValue : 32;
    };

    struct TStepPins
    {
        uint32_t t_step : 20;
        uint32_t reserved : 12;
    } __attribute__((packed));

    union Tstep
    {
        TStepPins tStepPins;
        uint32_t tStepValue : 32;
    };

    struct VActualPins
    {
        uint32_t vActual : 24;
        uint32_t reserved : 8;
    } __attribute__((packed));

    union VActual
    {
        VActualPins vActualPins;
        uint32_t vActualValue : 32;
    };
    struct SGResultPins
    {
        uint32_t sgResult : 10;
        uint32_t reserved : 22;
    } __attribute__((packed));

    union SGResult
    {
        SGResultPins sgResultPins;
        uint32_t sgResultValue : 32;
    };

    struct TCoolThresholdPins
    {
        uint32_t tCoolThreshold : 20;
        uint32_t reserved : 12;
    } __attribute__((packed));

    union TCoolThreshold
    {
        TCoolThresholdPins tCoolThresholdPins;
        uint32_t tCoolThrsValue : 32;
    };

    struct TPwmThresholdPins
    {
        uint32_t tPwThreshold : 20;
        uint32_t reserved : 12;
    } __attribute__((packed));

    union TPwmThreshold
    {
        TPwmThresholdPins tPwmThresholdPins;
        uint32_t tPwThrsValue : 32;
    };

    struct SGThresholdPins
    {
        uint32_t sgThreshold : 8;
        uint32_t reserved : 24;
    } __attribute__((packed));

    union SGThreshold
    {
        SGThresholdPins sgThresholdPins;
        uint32_t sgThrsValue : 32;
    };

    struct MSCurrentActualPins
    {
        uint32_t curA : 9;
        uint32_t reserved : 7;
        uint32_t curB : 9;
        uint32_t reserved1 : 7;
    } __attribute__((packed));

    union MSCurrentActual
    {
        MSCurrentActualPins msCurrentActualPins;
        uint32_t mscurActValue : 32;
    };

    struct IHoldRunPins
    {
        uint32_t ihold : 5;
        uint32_t reserved : 3;
        uint32_t irun : 5;
        uint32_t reserved1 : 3;
        uint32_t iholdDelay : 4;
        uint32_t reserved2 : 12;
    } __attribute__((packed));

    union IHoldRun
    {
        IHoldRunPins iholdrun;
        uint32_t iHoldRunValue : 32;
    };

    struct MSCountPins
    {
        uint32_t msCount : 10;
        uint32_t reserved : 22;
    } __attribute__((packed));

    union MSCount
    {
        MSCountPins msCountPins;
        uint32_t msCountValue : 32;
    };

    struct GConfigurationPins
    {
        uint32_t scaleAnalog : 1;
        uint32_t internalRsense : 1;
        uint32_t enSpreadCycle : 1;
        uint32_t shaft : 1;
        uint32_t indexOtpw : 1;
        uint32_t indexStep : 1;
        uint32_t pdnDisable : 1;
        uint32_t mstepRegSelect : 1;
        uint32_t multistepFilt : 1;
        uint32_t testMode : 1;
        uint32_t reserved : 22;
    } __attribute__((packed));

    union GConfigurations
    {
        GConfigurationPins gConfPins;
        uint32_t gConf : 32;
    };

    /**
     * Initialising Gconfigiration Register of TMC 2209
     * @return true if initilialisation is successful
     */
    bool gConfInit();

    /**
     * Sending Read Access Request datagram with register Address and Slave address. As well as
     * receiving Read Access Reply with the 32 bit data in the Register after a particular bit-time
     * set by SlaveConfig.
     * @param regAddr Register Address whose data neads to be read
     * @return returns the 32 bit data from the register
     */
    std::optional<uint32_t> readData(uint8_t regAddr);

    /**
     * Sending the write access request to write a particular register. If the write access is
     * acknowledged, the Interface transmission counter (IFCNT) is incremented. Check the IFCNT
     * register. If incremented, write access successful. If not, write access failed.
     * @param regAddr Register address to be written
     * @param data 32-bit data to be written
     * @return returns true if write access successful i.e Interface transmission counter
     * incremented. If not, return false.
     */
    bool writeData(uint8_t regAddr, uint32_t data);

    /**
     * @brief get the Interface transmission counter (IFCNT) to check if write access is successful
     * @return value of the transmission counter
     */
    std::optional<uint8_t> getTransmissionCount();

    /**
     * @brief Checks if the Overtemperature pre warning threshold is reached
     * @return returns true if threshold is exceeded. If not, returns false
     */
    std::optional<bool> getOvertempPrewarn();

    /**
     * @brief Checks if Overtemperature threshold is reached
     * @return returns true if threshold is exceeded. If not, returns false.
     */
    std::optional<bool> getOvertempStats();

    /**
     * @brief short to ground indicator phase A of motor
     * @return returns true if short to ground is detected on phase A. If not, returns false
     */
    std::optional<bool> short2GroundA();

    /**
     * @brief short to ground indicator phase B of motor
     * @return returns true if short to ground is detected on phase B. If not, returns false
     */
    std::optional<bool> short2GroundB();

    /**
     * @brief Short on low-side MOSFET detected on phase A
     * @return returns true if short on low-side MOSFET detected on phase A. If not, returns false
     */
    std::optional<bool> shortLsA();

    /**
     * @brief Short on low-side MOSFET detected on phase B
     * @return returns true if short on low-side MOSFET detected on phase B. If not, returns false
     */
    std::optional<bool> shortLsB();

    /**
     * @brief Open load detected on phase A.This is just an informative flag. The driver takes no
     * action upon it. False detection may occur in fast motion and standstill. Check during slow
     * motion, only.
     * @return returns true if open load detected on phase A. If not, returns false
     */
    std::optional<bool> openLoadA();

    /**
     * @brief Open load detected on phase B.This is just an informative flag. The driver takes no
     * action upon it. False detection may occur in fast motion and standstill. Check during slow
     * motion, only.
     * @return returns true if open load detected on phase B. If not, returns false
     */
    std::optional<bool> openLoadB();

    /**
     * @brief Checks if temperature threshold of 120°C is exceeded.
     * @return returns true if temperature threshold of 120°C is exceeded. If not, returns false.
     */
    std::optional<bool> tempExceeded120();

    /**
     * @brief Checks if temperature threshold of 143°C is exceeded.
     * @return returns true if temperature threshold of 143°C is exceeded. If not, returns false.
     */
    std::optional<bool> tempExceeded143();

    /**
     * @brief Checks if temperature threshold of 150°C is exceeded.
     * @return returns true if temperature threshold of 150°C is exceeded. If not, returns false.
     */
    std::optional<bool> tempExceeded150();

    /**
     * @brief Checks if temperature threshold of 157°C is exceeded.
     * @return returns true if temperature threshold of 157°C is exceeded. If not, returns false.
     */
    std::optional<bool> tempExceeded157();

    /**
     * @brief Indicates whether the driver is operating in StealthChop mode or SpreadCycle mode
     * @return returns true if driver operates in StealthChop mode. returns false if driver operates
     * in SpreadCycle mode
     */
    std::optional<bool> stealthChopIndicator();

    /**
     * @brief This flag indicates motor stand still in each operation mode.
     * @return returns true if motor is in standstill. If not, returns false.
     */
    std::optional<bool> standstillIndicator();

    /**
     *@brief Indicates that the IC has been reset since the last read access to GSTAT. All registers
     *have been cleared to reset values.
     * @return returns true if IC has been reset. If not, returns false.
     */
    std::optional<bool> icReset();

    /**
     * @brief Indicates, that the driver has been shut down due to overtemperature or short circuit
     * detection since the last read access. Read DrvStatus for details. The flag can only be
     * cleared when all error conditions are cleared.
     * @return returns true if driver has been shut down due to overtemperature or short circuit. If
     * not, returns false.
     */
    std::optional<bool> drvError();

    /**
     * @brief Indicates an undervoltage on the charge pump. The driver is disabled in this case.
     * This flag is not latched and thus does not need to be cleared.
     * @return returns true if there is undervoltage on the charge pump. If not, returns false.
     */
    std::optional<bool> underVoltageCp();

    /**
     * @brief Reading the GCONF register to get all the Global configuration flags
     * @return returns GConfigurations object with 32 bit data consisting of all the Global
     * configuration flags. return value of union GConfigurations can be used to access each
     * configuration flag separately.
     */
    std::optional<TMC2209::GConfigurations> getConfiguration();

    /**
     *@brief to set the global configuration flags in the GCONF register
     * @param gConfigurations union which can be used to set the values of different global
     *configuration flags
     * @return returns true if writing into GCONF register was successful. If not, return false.
     */
    bool setConfiguration(GConfigurations gConfigurations);

    /**
     *@brief to set the SENDDELAY for read access (time until the reply is sent) in the SLAVECONF
     *register
     * @param slaveConfig union which can be used to set the values of SENDDELAY to particular
     *number of bit times
     * @return returns true if writing into GCONF register was successful. If not, return false.
     */
    bool setSlaveConfig(SlaveConfig slaveConfig);

    /**
     * @brief Reading the state of all input pins available
     * @return  returns InputConfig object with 32 bit data consisting of states of all the input
     * pins. return value union InputConfig can be used to access state of each input pin
     * separately.
     */
    std::optional<TMC2209::InputConfig> getInputPinStatus();

    /**
     * @brief Reading the OTP_READ register. The OTP memory holds power up defaults for certain
     * registers.
     * @return returns OTPRead object with 32 bit data consisting of reset values for different OTP
     * functions. return value union OTPRead can be used to access reset values of each OTP function
     * separately.
     */
    std::optional<OTPRead> otpReadStatus();

    /**
     * @brief Programs the OTP memory. Writes into the OTP_PROG register.
     * @param otpProg union used to set the values for selecting a particular bit and otp function
     * to be programed
     * @return returns true if writing into OTP_PROG register was successful. If not, return false.
     */
    bool otpProgram(OTPProg otpProg);

    /**
     *@brief Calculates the CRC
     * @param buffer datagram (array of 8 bit)
     * @param len length of the datagram
     * @return returns the 8-bit CRC
     */
    void addCRC(uint8_t *datagram, uint8_t datagramLength);

    /**
     *@brief Reading the DRVSTATUS register. DRVSTATUS registers gives the Driver status flags.
     * @return returns DrvStats object with 32 bit data consisting of Driver status flags.
     * return value union DrvStats can be used to each Driver status flags separately.
     */
    std::optional<TMC2209::DrvStats> drvReadStatus();

    /**
     * @brief clears the reset flag that is set when IC has been reset since the last read access to
     * GSTAT.
     * @return returns true if the reset flag has been cleared successfully. If not, return false.
     */
    bool clearIcReset();

    /**
     * @brief clears the DRV error flag that is set when the driver has been shut down due to
     * overtemperature or short circuit detection since the last read access. Read DRVSTATUS for
     * details. This flag can only be cleared when all error conditions are cleared.
     * @return returns true if the DRV error flag has been cleared successfully. If not, return
     * false.
     */
    bool clearDrvErr();

    /**
     * @brief Reading FACTORY_CONF register. It has the values for lowest to highest clock frequency
     * (FCLKTRIM) and OTTRIM.
     * @return returns FactoryConfigurations object with 32 bit data consisting of values for lowest
     * to highest clock frequency (FCLKTRIM) and OTTRIM.
     */
    std::optional<TMC2209::FactoryConfigurations> getFactoryConfiguration();

    /**
     * @brief Writing into FACTORY_CONF register. It has the values for lowest to highest clock
     * frequency (FCLKTRIM) and OTTRIM.
     * @param factoryConfigurations union which can be used to set the values of different global
     * factory configuration settings like FCLKTRIM and OTTRIM
     * @return returns true if writing into FACTORY_CONF register was successful. If not, return
     * false.
     */
    bool setFactoryConfiguration(FactoryConfigurations factoryConfigurations);

    /**
     * @brief to set IHOLD_IRUN register with the values for IHOLD, IRUN and IHOLDDELAY
     * @param iHoldRun union which can be used to set the values of IHOLD ( Standstill current),
     * IRUN ( Motor run current) and IHOLDDELAY ( controls the number of clock cycles for motor
     * power down after TPOWERDOWN in increments of 2^18 clocks )
     * @return returns true if writing into IHOLD_IRUN register was successful. If not, return
     * false.
     */
    bool setIHoldRun(IHoldRun iHoldRun);

    /**
     * @brief to set the TPOWER_DOWN register
     * @param tPowerDown union which can be used to set the delay time from stand still (stst)
     * detection to motor current power down.
     * @return returns true if writing into TPOWER_DOWN register was successful. If not, return
     * false.
     */
    bool setTPowerDown(TPowerDown tPowerDown);

    /**
     * @brief reads the TSTEP register which gives the actual measured time between two 1/256
     * microsteps derived from the step input frequency in units of 1/fCLK
     * @return returns TStep object
     */
    std::optional<TMC2209::Tstep> getTStep();

    /**
     * @brief sets the upper velocity for StealthChop voltage PWM mode. When velocity exceeds the
     * limit set by TPWMTHRS, driver switches to SpreadCycle
     * @param tPwmThreshold union used to set upper velocity for StealthChop voltage PWM mode.
     * @return returns true if writing into TPWMTHRS register was successful. If not, return
     * false.
     */
    bool setTPwmThreshold(TPwmThreshold tPwmThreshold);

    /**
     * @brief sets the value for Vactual which allows moving the motor by UART control. For Normal
     * Operation, VACTUAL is 0. So, the driver reacts to the STEP input
     * @param vActual union which can be used to allow movement of motor by UART control
     * @return returns true if writing into VACTUAL register was successful. If not, return
     * false.
     */
    bool setVActual(VActual vActual);

    /**
     * @brief sets the lower threshold velocity for switching on smart energy Coolstep. At low
     * speeds, Coolstep is disabled where it cannot work reliably.
     * @param tCoolThreshold union used to set the lower threshold velocity for switching on smart
     * energy Coolstep
     * @return true if writing into TCOOLTHRS register was successful. If not, return
     * false.
     */
    bool setTCoolThreshold(TCoolThreshold tCoolThreshold);

    /**
     * @brief sets the detection threshold for stall. StallGuard value SG_RESULT becomes compared to
     * the double of this threshold. A stall is signaled with SG_RESULT <= SGTHRS*2
     * @param sgThreshold union used to set the detection threshold for stall.
     * @return true if writing into SGTHRS register was successful. If not, return
     * false.
     */
    bool setSGThreshold(SGThreshold sgThreshold);

    /**
     * @brief reads the SG_RESULT register which gives the StallGuard result. SG_RESULT becomes
     * updated with each fullstep, independent of TCOOLTHRS and SGTHRS.
     * @return SGResult object with StallGuard result.
     */
    std::optional<TMC2209::SGResult> getSGResult();

    bool setCoolConf(CoolConfiguration coolConfiguration);

    /**
     * @brief reads the MSCNT register which gives the Microstep counter. It Indicates actual
     * position in the microstep table.
     * @return MSCount object which allows determination of the motor position within the
     * electrical wave.
     */
    std::optional<TMC2209::MSCount> getMSCount();

    /**
     * @brief reads MSCURACT register which gives actual microstep current for motor phase A and B.
     * @return returns MSCurrentActual object with values from MSCURACT register.
     */
    std::optional<TMC2209::MSCurrentActual> getMSCurrent();

    /**
     * @brief reads the CHOPCONF register with different chopper and Driver Configurations.
     * @return returns ChopperConfig Object with values from CHOPCONF register.
     */
    std::optional<TMC2209::ChopperConfig> getChopConf();

    bool setChopConf(ChopperConfig chopperConfig);

    /**
     * @brief reads the PWMCONF register with StealthChop PWM chopper configurations.
     * @return returns PWMConfig Object with values from PWMCONF register.
     */
    std::optional<PWMConfig> getPWMConf();

    bool setPWMConf(PWMConfig pwmConfig);

    /**
     * @brief reads PWSCALE register which gives results of StealthChop amplitude regulator. These
     * values can be used to monitor automatic PWM amplitude scaling
     * @return returns PWMScale Object with values of PWSCALE_SUM ( Actual PWM duty cycle ) and
     * PWSCALE_AUTO ( 9 Bit signed offset added to the calculated PWM duty cycle )
     */
    std::optional<TMC2209::PWMScale> getPWMScale();

    /**
     * @brief reads the PWAUTO register which are automatically generated values that can be used
     * to determine a default / power up setting for PWGRAD and PWOFS.
     * @return returns PWMAuto Object with values of PWAUTO register
     */
    std::optional<TMC2209::PWMAuto> getPWMAuto();

    /**
     * @brief sets the slave address for TMC
     * @param slave Slave Address
     */
    void setSlaveAdress(uint8_t slave);

    /**
     * @brief activates CoolStep mode of TMC2209
     */
    void activateCoolStep();

    /**
     * @brief Initialises the Gconfiguration register and actiavtes coolstep
     * @return true if initialisation is successful. If not, return false.
     */
    bool tmcInit();

    /**
     * @brief Checks if a communication failure occurred
     * @return true     failure occurred
     * @return false    everything ok
     */
    [[nodiscard]] bool isCommFailure() const;
};