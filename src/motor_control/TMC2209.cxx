#include "TMC2209.hpp"

TMC2209::TMC2209(uint8_t slave, UART_HandleTypeDef *uartPeripherie)
    : slaveaddress(slave), uartPeripherie(uartPeripherie)
{
}

uint32_t TMC2209::readData(uint8_t regAddr)
{
    ReadRequest rdReqData{};
    rdReqData.syncAddr = 0x05;
    rdReqData.slaveAddr = slaveaddress;
    rdReqData.regAddr = regAddr;
    rdReqData.rw = 0;
    rdReqData.crc = 0;
    calcCRC(reinterpret_cast<uint8_t *>(&rdReqData), sizeof(rdReqData));
    ReadReqReply readReqReply{};
    DataReverseUnion dataReverseUnion{};
    DataReverse dataReverse{};

    HAL_HalfDuplex_EnableTransmitter(uartPeripherie);
    HAL_UART_Transmit(uartPeripherie, reinterpret_cast<uint8_t *>(&rdReqData), sizeof(rdReqData),
                      1000);
    HAL_HalfDuplex_EnableReceiver(uartPeripherie);
    HAL_UART_Receive(uartPeripherie, reinterpret_cast<uint8_t *>(&readReqReply),
                     sizeof(readReqReply), 2000);

    ReadReply readReply1{};
    readReply1 = readReqReply.readReply;
    dataReverse.data4 =
        (readReply1.data1); /* Reversing the bytes to get the Data in correct sequence */
    dataReverse.data3 = (readReply1.data2);
    dataReverse.data2 = (readReply1.data3);
    dataReverse.data1 = (readReply1.data4);
    dataReverseUnion.dataReverse = dataReverse;
    return dataReverseUnion.data;
}

bool TMC2209::writeData(uint8_t regAddr, uint32_t data)
{
    WriteData writeData{};
    WriteDataBits writeDataBits{};
    WriteAccess writeAccess{};
    DataReverseUnion dataReverseUnion{};
    DataReverse dataReverse{};
    uint8_t initialCount = 0;
    uint8_t finalCount = 0;
    writeData.data = data;
    writeDataBits = writeData.writeDataBits;
    dataReverse.data4 = writeDataBits.data1;
    dataReverse.data3 = writeDataBits.data2;
    dataReverse.data2 = writeDataBits.data3;
    dataReverse.data1 = writeDataBits.data4;
    dataReverseUnion.dataReverse = dataReverse;
    writeAccess.syncAddr = 0x05;
    writeAccess.slaveAddr = slaveaddress;
    writeAccess.regAddr = regAddr;
    writeAccess.rw = 1;
    writeAccess.data = dataReverseUnion.data;
    writeAccess.crc = 0;

    calcCRC(reinterpret_cast<uint8_t *>(&writeAccess), sizeof(writeAccess));
    initialCount = getTransmissionCount();
    HAL_HalfDuplex_EnableTransmitter(uartPeripherie);
    HAL_UART_Transmit(uartPeripherie, reinterpret_cast<uint8_t *>(&writeAccess),
                      sizeof(writeAccess), 1000);
    finalCount = getTransmissionCount();

    bool writeSuccessful = initialCount != finalCount;
    commOk = writeSuccessful && commOk;
    return writeSuccessful;
    /* Checking the Transmission count value before and after write operation to check if Write
     * Access was successful. When the write access is successful, the Transmission count
     * (Interface Transmission Counter register) is incremented. */
}

TMC2209::DrvStats TMC2209::drvReadStatus()
{
    DrvStats drvStats{};
    drvStats.drvStatus = readData(DrvStatus);
    return drvStats;
}

TMC2209::GConfigurations TMC2209::getConfiguration()
{
    GConfigurations gConfigurations{};
    gConfigurations.gConf = readData(Gconf);
    return gConfigurations;
}

TMC2209::FactoryConfigurations TMC2209::getFactoryConfiguration()
{
    FactoryConfigurations factoryConfigurations{};
    factoryConfigurations.factConf = readData(FactoryConf);
    return factoryConfigurations;
}

bool TMC2209::setFactoryConfiguration(FactoryConfigurations factoryConfigurations)
{
    return writeData(FactoryConf, factoryConfigurations.factConf);
}

TMC2209::InputConfig TMC2209::getInputPinStatus()
{
    InputConfig inputConfig{};
    inputConfig.inputConf = readData(Ioin);
    return inputConfig;
}

TMC2209::OTPRead TMC2209::otpReadStatus()
{
    OTPRead otpReadPins{};
    otpReadPins.otpRead = readData(OtpRead);
    return otpReadPins;
}

TMC2209::ChopperConfig TMC2209::getChopConf()
{
    ChopperConfig chopperConfig{};
    chopperConfig.chopperConfValue = readData(ChopConf);
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

uint8_t TMC2209::getTransmissionCount()
{
    IFCount ifCount{};
    ifCount.ifCount = readData(Ifcnt);
    return ifCount.ifCountPins.count;
}

TMC2209::Tstep TMC2209::getTStep()
{
    Tstep tstep{};
    tstep.tStepValue = readData(TstepRegister);
    return tstep;
}

TMC2209::MSCount TMC2209::getMSCount()
{
    MSCount msCount{};
    msCount.msCountValue = readData(MsCnt);
    return msCount;
}

TMC2209::MSCurrentActual TMC2209::getMSCurrent()
{
    MSCurrentActual msCurrentActual{};
    msCurrentActual.mscurActValue = readData(MscurAct);
    return msCurrentActual;
}

TMC2209::SGResult TMC2209::getSGResult()
{
    SGResult sgResult{};
    sgResult.sgResultValue = readData(SgResult);
    return sgResult;
}

bool TMC2209::getOvertempPrewarn()
{
    DrvStats drvStatus{};
    drvStatus.drvStatus = readData(DrvStatus);
    return drvStatus.drvStatsRegister.overtemperaturePreWarning;
}

bool TMC2209::getOvertempStats()
{
    DrvStats drvStatus{};
    drvStatus.drvStatus = readData(DrvStatus);
    return drvStatus.drvStatsRegister.overtemperatureFlag;
}

bool TMC2209::short2GroundA()
{
    DrvStats drvStatus{};
    drvStatus.drvStatus = readData(DrvStatus);
    return drvStatus.drvStatsRegister.shortToGndA;
}

bool TMC2209::short2GroundB()
{
    DrvStats drvStatus{};
    drvStatus.drvStatus = readData(DrvStatus);
    return drvStatus.drvStatsRegister.shortToGndB;
}

bool TMC2209::shortLsA()
{
    DrvStats drvStatus{};
    drvStatus.drvStatus = readData(DrvStatus);
    return drvStatus.drvStatsRegister.shortLsA;
}

bool TMC2209::shortLsB()
{
    DrvStats drvStatus{};
    drvStatus.drvStatus = readData(DrvStatus);
    return drvStatus.drvStatsRegister.shortLsB;
}

bool TMC2209::openLoadA()
{
    DrvStats drvStatus{};
    drvStatus.drvStatus = readData(DrvStatus);
    return drvStatus.drvStatsRegister.openLoadA;
}

bool TMC2209::openLoadB()
{
    DrvStats drvStatus{};
    drvStatus.drvStatus = readData(DrvStatus);
    return drvStatus.drvStatsRegister.openLoadB;
}

bool TMC2209::tempExceeded120()
{
    DrvStats drvStatus{};
    drvStatus.drvStatus = readData(DrvStatus);
    return drvStatus.drvStatsRegister.tempExceeded120;
}

bool TMC2209::tempExceeded143()
{
    DrvStats drvStatus{};
    drvStatus.drvStatus = readData(DrvStatus);
    return drvStatus.drvStatsRegister.tempExceeded143;
}

bool TMC2209::tempExceeded150()
{
    DrvStats drvStatus{};
    drvStatus.drvStatus = readData(DrvStatus);
    return drvStatus.drvStatsRegister.tempExceeded150;
}

bool TMC2209::tempExceeded157()
{
    DrvStats drvStatus{};
    drvStatus.drvStatus = readData(DrvStatus);
    return drvStatus.drvStatsRegister.tempExceeded157;
}

bool TMC2209::stealthChopIndicator()
{
    DrvStats drvStatus{};
    drvStatus.drvStatus = readData(DrvStatus);
    return drvStatus.drvStatsRegister.stealthChopIndicator;
}

bool TMC2209::standstillIndicator()
{
    DrvStats drvStatus{};
    drvStatus.drvStatus = readData(DrvStatus);
    return drvStatus.drvStatsRegister.standstillIndicator;
}

bool TMC2209::icReset()
{
    GStats gStats{};
    gStats.gStatus = readData(Gstat);
    return gStats.gStatsPins.reset;
}

bool TMC2209::drvError()
{
    GStats gStats{};
    gStats.gStatus = readData(Gstat);
    return gStats.gStatsPins.drvErr;
}

bool TMC2209::underVoltageCp()
{
    GStats gStats{};
    gStats.gStatus = readData(Gstat);
    return gStats.gStatsPins.uvCp;
}

void TMC2209::calcCRC(uint8_t *datagram, uint8_t datagramLength)
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

TMC2209::PWMConfig TMC2209::getPWMConf()
{
    PWMConfig pwmConfig{};
    pwmConfig.pwmConfValue = readData(PwmConf);
    return pwmConfig;
}

bool TMC2209::setPWMConf(TMC2209::PWMConfig pwmConfig)
{
    return writeData(PwmConf, pwmConfig.pwmConfValue);
}

TMC2209::PWMScale TMC2209::getPWMScale()
{
    PWMScale pwmScale{};
    pwmScale.pwscaleValue = readData(PwScale);
    return pwmScale;
}

TMC2209::PWMAuto TMC2209::getPWMAuto()
{
    PWMAuto pwmAuto{};
    pwmAuto.pwautoValue = readData(PwAuto);
    return pwmAuto;
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