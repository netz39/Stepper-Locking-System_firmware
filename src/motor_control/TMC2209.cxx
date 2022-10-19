#include "TMC2209.hpp"

TMC2209::TMC2209(uint8_t slave, UartAccessor &uartAccessor)
    : slaveaddress(slave), uartAccessor(uartAccessor)
{
}

std::optional<uint32_t> TMC2209::readData(uint8_t regAddr)
{
    ReadRequest rdReqData{};
    rdReqData.syncAddr = 0x05;
    rdReqData.slaveAddr = slaveaddress;
    rdReqData.regAddr = regAddr;
    rdReqData.rw = 0;
    rdReqData.crc = 0;
    addCRC(reinterpret_cast<uint8_t *>(&rdReqData), sizeof(rdReqData));

    ReadReqReply readReqReply{};

    uartAccessor.beginTransaction();
    uartAccessor.halfDuplexSwitchToTx();

    if (!uartAccessor.transmit(reinterpret_cast<uint8_t *>(&rdReqData), sizeof(rdReqData)))
    {
        uartAccessor.endTransaction();
        commOk = false;
        return {};
    }

    uartAccessor.halfDuplexSwitchToRx();
    commOk = uartAccessor.receive(reinterpret_cast<uint8_t *>(&readReqReply), sizeof(readReqReply));
    uartAccessor.endTransaction();

    if (!commOk)
    {
        return {};
    }

    // reversing the bytes to get the data in correct sequence
    DataReverse dataReverse{};
    dataReverse.data4 = readReqReply.readReply.data1;
    dataReverse.data3 = readReqReply.readReply.data2;
    dataReverse.data2 = readReqReply.readReply.data3;
    dataReverse.data1 = readReqReply.readReply.data4;

    DataReverseUnion dataReverseUnion{};
    dataReverseUnion.dataReverse = dataReverse;
    return dataReverseUnion.data;
}

bool TMC2209::writeData(uint8_t regAddr, uint32_t data)
{
    WriteData writeData{};
    writeData.data = data;

    WriteDataBits writeDataBits{};
    writeDataBits = writeData.writeDataBits;

    DataReverse dataReverse{};
    dataReverse.data4 = writeDataBits.data1;
    dataReverse.data3 = writeDataBits.data2;
    dataReverse.data2 = writeDataBits.data3;
    dataReverse.data1 = writeDataBits.data4;

    DataReverseUnion dataReverseUnion{};
    dataReverseUnion.dataReverse = dataReverse;

    WriteAccess writeAccess{};
    writeAccess.syncAddr = 0x05;
    writeAccess.slaveAddr = slaveaddress;
    writeAccess.regAddr = regAddr;
    writeAccess.rw = 1;
    writeAccess.data = dataReverseUnion.data;
    writeAccess.crc = 0;
    addCRC(reinterpret_cast<uint8_t *>(&writeAccess), sizeof(writeAccess));

    auto initialCount = getTransmissionCount();
    if (!initialCount.has_value())
    {
        // transmit failed
        commOk = false;
        return false;
    }

    uartAccessor.beginTransaction();
    uartAccessor.halfDuplexSwitchToTx();

    bool success =
        uartAccessor.transmit(reinterpret_cast<uint8_t *>(&writeAccess), sizeof(writeAccess));
    uartAccessor.endTransaction();

    if (!success)
    {
        // transmit failed
        commOk = false;
        return false;
    }

    auto finalCount = getTransmissionCount();
    if (!initialCount.has_value())
    {
        // transmit failed
        commOk = false;
        return false;
    }

    bool writeSuccessful = initialCount.value_or(false) != finalCount.value_or(false);
    commOk = writeSuccessful;
    return writeSuccessful;
    // Checking the Transmission count value before and after write operation to check if Write
    // Access was successful. When the write access is successful, the Transmission count
    // (Interface Transmission Counter register) is incremented.
}

std::optional<TMC2209::DrvStats> TMC2209::drvReadStatus()
{
    auto result = readData(DrvStatus);
    if (!result.has_value())
        return {};

    DrvStats drvStats{};
    drvStats.drvStatus = result.value();
    return drvStats;
}

std::optional<TMC2209::GConfigurations> TMC2209::getConfiguration()
{
    auto result = readData(Gconf);
    if (!result.has_value())
        return {};

    GConfigurations gConfigurations{};
    gConfigurations.gConf = result.value();
    return gConfigurations;
}

std::optional<TMC2209::FactoryConfigurations> TMC2209::getFactoryConfiguration()
{
    auto result = readData(FactoryConf);
    if (!result.has_value())
        return {};

    FactoryConfigurations factoryConfigurations{};
    factoryConfigurations.factConf = result.value();
    return factoryConfigurations;
}

bool TMC2209::setFactoryConfiguration(FactoryConfigurations factoryConfigurations)
{
    return writeData(FactoryConf, factoryConfigurations.factConf);
}

std::optional<TMC2209::InputConfig> TMC2209::getInputPinStatus()
{
    auto result = readData(Ioin);
    if (!result.has_value())
        return {};

    InputConfig inputConfig{};
    inputConfig.inputConf = result.value();
    return inputConfig;
}

std::optional<TMC2209::OTPRead> TMC2209::otpReadStatus()
{
    auto result = readData(OtpRead);
    if (!result.has_value())
        return {};

    OTPRead otpReadPins{};
    otpReadPins.otpRead = result.value();
    return otpReadPins;
}

std::optional<TMC2209::ChopperConfig> TMC2209::getChopConf()
{
    auto result = readData(ChopConf);
    if (!result.has_value())
        return {};

    ChopperConfig chopperConfig{};
    chopperConfig.chopperConfValue = result.value();
    return chopperConfig;
}

bool TMC2209::setChopConf(ChopperConfig chopperConfig)
{
    return writeData(ChopConf, chopperConfig.chopperConfValue);
}

bool TMC2209::setConfiguration(GConfigurations gConfigurations)
{
    return writeData(Gconf, gConfigurations.gConf);
}

bool TMC2209::setVActual(VActual vActual)
{
    return writeData(Vactual, vActual.vActualValue);
}

bool TMC2209::setSlaveConfig(SlaveConfig slaveConfig)
{

    return writeData(Slaveconf, slaveConfig.slaveConfig);
}

bool TMC2209::setTCoolThreshold(TCoolThreshold tCoolThreshold)
{
    return writeData(TcoolThrs, tCoolThreshold.tCoolThrsValue);
}

bool TMC2209::setCoolConf(CoolConfiguration coolConfiguration)
{
    return writeData(CoolConf, coolConfiguration.coolConfValue);
}

bool TMC2209::otpProgram(OTPProg otpProg)
{
    return writeData(OtpProgram, otpProg.otpProg);
}

bool TMC2209::setSGThreshold(SGThreshold sgThreshold)
{
    return writeData(SgThrs, sgThreshold.sgThrsValue);
}

bool TMC2209::setIHoldRun(IHoldRun iHoldRun)
{
    return writeData(IholdIrun, iHoldRun.iHoldRunValue);
}

bool TMC2209::setTPwmThreshold(TPwmThreshold tPwmThreshold)
{
    return writeData(TpwmThrs, tPwmThreshold.tPwThrsValue);
}

bool TMC2209::setTPowerDown(TPowerDown tPowerDown)
{
    return writeData(TpowerDown, tPowerDown.tPowerDownValue);
}

bool TMC2209::clearDrvErr()
{
    GStats gStats{};
    GStatsPins gStatsPins{};
    gStatsPins.drvErr = 1;
    gStats.gStatsPins = gStatsPins;
    return writeData(Gstat, gStats.gStatus);
}

bool TMC2209::clearIcReset()
{
    GStats gStats{};
    GStatsPins gStatsPins{};
    gStatsPins.reset = 1;
    gStats.gStatsPins = gStatsPins;
    return writeData(Gstat, gStats.gStatus);
}

std::optional<uint8_t> TMC2209::getTransmissionCount()
{
    auto result = readData(Ifcnt);
    if (!result.has_value())
        return {};

    IFCount ifCount{};
    ifCount.ifCount = result.value();
    return {static_cast<uint8_t>(ifCount.ifCountPins.count)};
}

std::optional<TMC2209::Tstep> TMC2209::getTStep()
{
    auto result = readData(TstepRegister);
    if (!result.has_value())
        return {};

    Tstep tstep{};
    tstep.tStepValue = result.value();
    return tstep;
}

std::optional<TMC2209::MSCount> TMC2209::getMSCount()
{
    auto result = readData(MsCnt);
    if (!result.has_value())
        return {};

    MSCount msCount{};
    msCount.msCountValue = result.value();
    return msCount;
}

std::optional<TMC2209::MSCurrentActual> TMC2209::getMSCurrent()
{
    auto result = readData(MscurAct);
    if (!result.has_value())
        return {};

    MSCurrentActual msCurrentActual{};
    msCurrentActual.mscurActValue = result.value();
    return msCurrentActual;
}

std::optional<TMC2209::SGResult> TMC2209::getSGResult()
{
    auto result = readData(SgResult);
    if (!result.has_value())
        return {};

    SGResult sgResult{};
    sgResult.sgResultValue = result.value();
    return sgResult;
}

std::optional<bool> TMC2209::getOvertempPrewarn()
{
    auto result = readData(DrvStatus);
    if (!result.has_value())
        return {};

    DrvStats drvStatus{};
    drvStatus.drvStatus = result.value();
    return {drvStatus.drvStatsRegister.overtemperaturePreWarning != 0};
}

std::optional<bool> TMC2209::getOvertempStats()
{
    auto result = readData(DrvStatus);
    if (!result.has_value())
        return {};

    DrvStats drvStatus{};
    drvStatus.drvStatus = result.value();
    return {drvStatus.drvStatsRegister.overtemperatureFlag != 0};
}

std::optional<bool> TMC2209::short2GroundA()
{
    auto result = readData(DrvStatus);
    if (!result.has_value())
        return {};

    DrvStats drvStatus{};
    drvStatus.drvStatus = result.value();
    return {drvStatus.drvStatsRegister.shortToGndA != 0};
}

std::optional<bool> TMC2209::short2GroundB()
{
    auto result = readData(DrvStatus);
    if (!result.has_value())
        return {};

    DrvStats drvStatus{};
    drvStatus.drvStatus = result.value();
    return {drvStatus.drvStatsRegister.shortToGndB != 0};
}

std::optional<bool> TMC2209::shortLsA()
{
    auto result = readData(DrvStatus);
    if (!result.has_value())
        return {};

    DrvStats drvStatus{};
    drvStatus.drvStatus = result.value();
    return {drvStatus.drvStatsRegister.shortLsA != 0};
}

std::optional<bool> TMC2209::shortLsB()
{
    auto result = readData(DrvStatus);
    if (!result.has_value())
        return {};

    DrvStats drvStatus{};
    drvStatus.drvStatus = result.value();
    return {drvStatus.drvStatsRegister.shortLsB != 0};
}

std::optional<bool> TMC2209::openLoadA()
{
    auto result = readData(DrvStatus);
    if (!result.has_value())
        return {};

    DrvStats drvStatus{};
    drvStatus.drvStatus = result.value();
    return {drvStatus.drvStatsRegister.openLoadA != 0};
}

std::optional<bool> TMC2209::openLoadB()
{
    auto result = readData(DrvStatus);
    if (!result.has_value())
        return {};

    DrvStats drvStatus{};
    drvStatus.drvStatus = result.value();
    return {drvStatus.drvStatsRegister.openLoadB != 0};
}

std::optional<bool> TMC2209::tempExceeded120()
{
    auto result = readData(DrvStatus);
    if (!result.has_value())
        return {};
    DrvStats drvStatus{};
    drvStatus.drvStatus = result.value();
    return {drvStatus.drvStatsRegister.tempExceeded120 != 0};
}

std::optional<bool> TMC2209::tempExceeded143()
{
    auto result = readData(DrvStatus);
    if (!result.has_value())
        return {};

    DrvStats drvStatus{};
    drvStatus.drvStatus = result.value();
    return {drvStatus.drvStatsRegister.tempExceeded143 != 0};
}

std::optional<bool> TMC2209::tempExceeded150()
{
    auto result = readData(DrvStatus);
    if (!result.has_value())
        return {};

    DrvStats drvStatus{};
    drvStatus.drvStatus = result.value();
    return {drvStatus.drvStatsRegister.tempExceeded150 != 0};
}

std::optional<bool> TMC2209::tempExceeded157()
{
    auto result = readData(DrvStatus);
    if (!result.has_value())
        return {};

    DrvStats drvStatus{};
    drvStatus.drvStatus = result.value();
    return {drvStatus.drvStatsRegister.tempExceeded157 != 0};
}

std::optional<bool> TMC2209::stealthChopIndicator()
{
    auto result = readData(DrvStatus);
    if (!result.has_value())
        return {};

    DrvStats drvStatus{};
    drvStatus.drvStatus = result.value();
    return {drvStatus.drvStatsRegister.stealthChopIndicator != 0};
}

std::optional<bool> TMC2209::standstillIndicator()
{
    auto result = readData(DrvStatus);
    if (!result.has_value())
        return {};

    DrvStats drvStatus{};
    drvStatus.drvStatus = result.value();
    return {drvStatus.drvStatsRegister.standstillIndicator != 0};
}

std::optional<bool> TMC2209::icReset()
{
    auto result = readData(Gstat);
    if (!result.has_value())
        return {};

    GStats gStats{};
    gStats.gStatus = result.value();
    return {gStats.gStatsPins.reset != 0};
}

std::optional<bool> TMC2209::drvError()
{
    auto result = readData(Gstat);
    if (!result.has_value())
        return {};

    GStats gStats{};
    gStats.gStatus = result.value();
    return {gStats.gStatsPins.drvErr != 0};
}

std::optional<bool> TMC2209::underVoltageCp()
{
    auto result = readData(Gstat);
    if (!result.has_value())
        return {};

    GStats gStats{};
    gStats.gStatus = result.value();
    return {gStats.gStatsPins.uvCp != 0};
}

void TMC2209::addCRC(uint8_t *datagram, uint8_t datagramLength)
{
    int i, j;
    uint8_t *crc = datagram + (datagramLength - 1); // CRC located in last byte of message
    uint8_t currentByte;
    *crc = 0;
    for (i = 0; i < (datagramLength - 1); i++)
    {
        currentByte = datagram[i];
        for (j = 0; j < 8; j++)
        {
            // Execute for all bytes of a message // Retrieve a byte to be sent from Array
            if ((*crc >> 7) ^ (currentByte & 0x01))
            {
                // update CRC based result of XOR operation
                *crc = (*crc << 1) ^ 0x07;
            }
            else
            {
                *crc = (*crc << 1);
            }
            currentByte = currentByte >> 1;
        } // for CRC bit } // for message byte
    }
}

std::optional<TMC2209::PWMConfig> TMC2209::getPWMConf()
{
    auto result = readData(PwmConf);
    if (!result.has_value())
        return {};

    PWMConfig pwmConfig{};
    pwmConfig.pwmConfValue = result.value();
    return {pwmConfig};
}

bool TMC2209::setPWMConf(TMC2209::PWMConfig pwmConfig)
{
    return writeData(PwmConf, pwmConfig.pwmConfValue);
}

std::optional<TMC2209::PWMScale> TMC2209::getPWMScale()
{
    auto result = readData(PwScale);
    if (!result.has_value())
        return {};

    PWMScale pwmScale{};
    pwmScale.pwscaleValue = result.value();
    return {pwmScale};
}

std::optional<TMC2209::PWMAuto> TMC2209::getPWMAuto()
{
    auto result = readData(PwAuto);
    if (!result.has_value())
        return {};

    PWMAuto pwmAuto{};
    pwmAuto.pwautoValue = result.value();
    return {pwmAuto};
}

bool TMC2209::gConfInit()
{
    GConfigurations gConf{};

    gConf.gConfPins.scaleAnalog = 1;    // using the VREF
    gConf.gConfPins.internalRsense = 1; // if we are using VREF as current reference.
    gConf.gConfPins.enSpreadCycle = 1;  // enabling Spreadcycle
    gConf.gConfPins.shaft = 0;          // normal motor direction
    gConf.gConfPins.indexOtpw = 0;      // index shows the microstep position of sequencer
    gConf.gConfPins.indexStep = 0;      // index output as selected by indexOtpw
    gConf.gConfPins.pdnDisable = 1;     // set for UART interface.
    gConf.gConfPins.mstepRegSelect = 0; // microstep resolution with MS1 and MS2
    gConf.gConfPins.multistepFilt = 0;  // no filtering of Step pulses
    gConf.gConfPins.testMode = 0;       // normal operation
    return setConfiguration(gConf);
}

void TMC2209::setSlaveAdress(uint8_t slave)
{
    slaveaddress = slave;
}

bool TMC2209::isCommFailure() const
{
    return !commOk;
}

void TMC2209::activateCoolStep()
{
    // digitalWrite(StepperEnablePin, 0);

    CoolConfiguration coolConf{};
    coolConf.coolConfPins.seup = 0x03;
    coolConf.coolConfPins.sedn = 0x00;
    coolConf.coolConfPins.seimin = 0;
    coolConf.coolConfPins.semin = 0x02;
    coolConf.coolConfPins.semax = 0x0E;
    setCoolConf(coolConf); // Setting the increment step, decrement step of
                           // current , minimum current, minimum and maximum
                           // threshold values in which the coolStep functions
    TCoolThreshold tCoolThreshold{};
    tCoolThreshold.tCoolThresholdPins.tCoolThreshold = 0x01;
    // Setting lower threshold velocity for switching on smart energy CoolStep
    setTCoolThreshold(tCoolThreshold);
    SGThreshold sgThreshold{};
    sgThreshold.sgThresholdPins.sgThreshold = 0x05;
    setSGThreshold(sgThreshold); // Setting SGThreshold to half the value of SG_RESULT
                                 // which is obtained just before stalling.
    TPwmThreshold tPwmThreshold{};
    tPwmThreshold.tPwmThresholdPins.tPwThreshold = 0x8000;
    setTPwmThreshold(tPwmThreshold); // Setting Upper velocity threshold value for
                                     // CoolStep. Above it, CoolStep is disabled.
}

bool TMC2209::tmcInit()
{
    // activateCoolStep();
    return gConfInit();
}