#pragma once
#include "FreeRTOS.h"
#include "as5600_constants.hpp"
#include "rtos_accessor.hpp"
#include "task.h"
#include <utility>

/**
 * @brief Driver for AMS AS5600 Magnetic Rotary Encoder
 * Only one instance per accessor allowed because you can't change the device's
 * address
 *
 */

namespace AS5600
{
class AS5600
{
public:
    enum class Voltage : uint8_t
    {
        ThreePointThree,
        Five
    };

    enum class Variant : uint8_t
    {
        AS5600,
        AS5600L
    };

    /**
     * @brief Construct a new AS5600 driver
     *
     *
     * @param accessor  i2c bus access
     * @param voltage Volage the device is fed
     * @param createTask  true to create a task that handles polling the sensor
     * @param taskName name of task
     * @param prio priority of task
     */
    AS5600(i2c::RtosAccessor &accessor, Voltage voltage, Variant variant, bool createTask,
           const char *taskName = "(no task)", UBaseType_t prio = 1);

    /**
     * @brief Destroy the AS5600 object
     * If task was created, it will be deleted. If there is no support for task deletion
     * abort() will be called
     */
    ~AS5600();

    bool operator==(const AS5600 &other) const;

    /**
     * @brief Initializes the internal state by synching with device.
     * Initialize interal values to devices default values
     * @return i2c bus error
     */
    bool init();

    /**
     * @brief Polls the sensor, does housekeeping. ONLY USE WHEN NO TASK WAS CHOSEN
     * Polling rates will not be impacted on how fast you call this function
     * If communication is interrupted. isOK will return false and the thread will try to
     * regain connection to the sensor and reconfigure it with previous parameters.
     * In that case this function will block.
     *
     * @param blocking true if run should block to wait for polling rates. false otherwise
     */
    void run(bool blocking);

    enum class PowerMode : uint8_t
    {
        Normal = 0, // !< max 6.5mA, 2ms polling time

        /* low power modes */
        LPM1, //!< max 3.4mA, polling time = 5ms
        LPM2, //!< max 1.8mA, polling time = 20ms
        LPM3  //!< max 1.5mA, polling time = 100ms
    };

    enum class HysteresisMode : uint8_t
    {
        Off = 0,
        LSB1, // 1 Least significant bit
        LSB2,
        LSB3
    };

    enum class SlowFilterMode : uint8_t
    {
        x16 = 0, // forced in low power mode, setting has no impact
        x8,
        x4,
        x2
    };

    enum class FastFilterThreshold : uint8_t
    {
        slowFilter = 0,
        LSB6,
        LSB7,
        LSB9,
        LSB18,
        LSB21,
        LSB24,
        LSB10
    };

    /**
     * @brief Changes settings of device. Blocking. Waits until a new angle sample is received.
     *
     * @param pwrMode
     * @param hystMode
     * @param sfMode
     * @param ffth
     * @param watchdog
     * @return true  success
     * @return false i2c bus error
     */
    bool configureDevice(PowerMode pwrMode, HysteresisMode hystMode, SlowFilterMode sfMode,
                         FastFilterThreshold ffth, bool watchdog);

    bool configurePowerMode(PowerMode pwrMode);

    /**
     * @brief Checks if the magnet is at the correct distance. Non-blocking.
     * Done at an interval in run()
     *
     * @return float negative: too close, 0-1 just right, >1 too far away
     */
    [[nodiscard]] float getMagnetStatus() const
    {
        return magnetStatus_;
    }

    /**
     * @brief Checks if the magnet is too close.
     *
     * @return true    magnet is too close
     * @return false   magnet is not too close
     */
    [[nodiscard]] bool isMagnetTooClose() const
    {
        return magnetStatus_ < MagnetTooCloseLimit;
    }

    /**
     * @brief Checks if the magnet is too far away.
     *
     * @return true     magnet is too far away
     * @return false    magnet is not too far away
     */
    [[nodiscard]] bool isMagnetTooFarAway() const
    {
        return magnetStatus_ > MagnetTooFarLimit;
    }

    /**
     * @brief Checks if the communication is ok.
     *
     * @return true     communication is ok
     * @return false    communication is interrupted
     */
    [[nodiscard]] bool isCommunicationOK() const
    {
        return !commFail_;
    }

    /**
     * @brief Checks if communication and magnet are ok. Non-blocking.
     * See run() for additional details
     * Also returns false when run() wasn't called at least once
     *
     * @return true  green across the board
     * @return false  communication interrupted
     */
    [[nodiscard]] bool isOK() const
    {
        return !isMagnetTooClose() && !isMagnetTooFarAway() && isCommunicationOK() && !reconnecting_;
    }

    /**
     * @brief Checks if the driver is successfully initialized. Non-blocking.
     * Poll this function until it returns true when createTask is used in constructor.
     * When createTask is not used, just wait for init() to finish.
     *
     * @return true
     * @return false
     */
    [[nodiscard]] bool isInitialized() const
    {
        return initialized_;
    }

    /**
     * @brief Set the start and stop of angular range. Blocking. Waits for new angle sample.
     *
     * @param rawStartPos 0 - 4095
     * @param rawStopPos 0 - 4095
     * @return true no error
     * @return false communication interrupted. See run() for additional details
     */
    bool setStartStopPosition(uint16_t rawStartPos, uint16_t rawStopPos);

    /**
     * @brief Get the start and stop of angular range. Non-blocking
     *
     * @return std::pair<uint16_t, uint16_t> <start, stop>
     */
    std::pair<uint16_t, uint16_t> getStartStopPosition() const
    {
        return std::make_pair(startPosition_, stopPosition_);
    }

    /**
     * @brief Get the maximum angular range. Non-blocking
     *
     * @return maximum in radiants 1/10 Pi - 2Pi
     */
    float getMaximumAngle() const
    {
        return maxAngle_;
    }

    /**
     * @brief Set the maximum angular range, value must be greater than 18 degrees. Blocking. Will
     * wait for new angle sample
     * @param maxAngle value between 1/10 Pi - 2Pi
     * @return true on success,
     * @return false on communication failure. See run() for additional details
     */
    bool setMaximumAngle(float maxAngle);

    /**
     * @brief Get the unscaled and unmodified angle value. Blocking.
     * This function will block as run() will only retrieve the scaled angle
     * value. Check isOK() afterwards. Will return 0 if communication failed.
     * @return 0 - 4095
     * @
     */
    uint16_t getRawAngle();

    /**
     * @brief Get the angle scaled from 0 to 2π. Non-blocking
     *
     * 1. startPosition -> stopPosition is scaled to go from 0 - 2π
     * 2. full range is then scaled down by maximum angle
     * 0 - 2π -> 0 - maximum angle
     *
     * @return current scaled angle in radians
     */
    [[nodiscard]] float getAngleScaledRadian() const
    {
        return angleScaledRadian_;
    }

    /**
     * @brief Get the angle scaled from 0 to 4095. Non-blocking
     *
     * 1. startPosition -> stopPosition is scaled to go from 0 - 4095
     * 2. full range is then scaled down by maximum angle
     * 0 - 4095 -> 0 - maximum angle
     *
     * @return current scaled angle
     */
    [[nodiscard]] uint16_t getAngleScaled() const
    {
        return angleScaled_;
    }

private:
    i2c::RtosAccessor &accessor_;
    const Voltage voltage_;
    const uint8_t deviceAddress_;
    TaskHandle_t task_ = nullptr;
    bool commFail_ = true;
    bool reconnecting_ = false;
    bool initialized_ = false;

    PowerMode pwrMode_;
    HysteresisMode hystMode_;
    SlowFilterMode sfMode_;
    FastFilterThreshold ffth_;
    bool watchdog_ = true;

    float magnetStatus_ = 0.5;
    uint16_t rawAngle_ = 0;
    uint16_t angleScaled_ = 0;
    float angleScaledRadian_ = 0;
    float maxAngle_ = 0;
    uint16_t startPosition_ = 0;
    uint16_t stopPosition_ = 4095;
    uint32_t lastAnglePolling_ = 0;
    uint32_t lastMagnetPolling_ = 0;
    static constexpr uint32_t MagnetPollingTime = 500; // ms
    static constexpr float MagnetTooFarLimit = 1.f;
    static constexpr float MagnetTooCloseLimit = 0.f;
    static constexpr float MaxValueThreePointThreeVolt = 128.f;
    static constexpr float MaxValueFiveVolt = 255.f;

    /**
     * @brief reads / writes two bytes to a register. Accepts / Returns data in little endian order.
     *
     * @param dir
     * @param reg
     * @param data
     */
    void rwTwoBytes(Direction dir, RegisterTwoBytes reg, uint16_t *data);
    void rwOneByte(Direction dir, RegisterOneByte reg, uint8_t *data);

    /**
     * @brief FreeeRTOS task's entry function
     *
     * @param context
     */
    [[noreturn]] static void taskEntry(void *context);

    /**
     * @brief sensor is bit-endian internally. using this to swap to little-endian
     * for all two byte registers
     *
     */
    static void swapBytes(uint16_t &);

    /**
     * @brief Reads out the scaled angle and writes it to angle_
     *
     * @return true communication ok
     * @return false communicaiton failed
     */
    bool synchronizeScaledAngle();

    /**
     * @brief Translates power modes to polling times for freertos
     *
     * @param pwrMode
     * @return TickType_t
     */
    TickType_t getPollingTime(PowerMode pwrMode);
};
} // namespace AS5600