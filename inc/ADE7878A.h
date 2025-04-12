#pragma once

#include "i2c_interface.h"
#include <array>
#include <chrono>
#include <memory>
#include "exceptions.h"

struct PowerReadings {
    double voltage;
    std::array<double, 3> current;      // For 3 appliances
    std::array<double, 3> activePower;
    std::array<double, 3> reactivePower;
    std::array<double, 3> apparentPower;
    double frequency;
    uint64_t timestamp;
};

class ADE7878A {
public:
    ADE7878A();
    ~ADE7878A();

    void initialize(I2CInterface* i2c, const std::array<double, 3>& calibrationFactors);
    bool isInitialized() const { return initialized_; }

    PowerReadings readPowerMeasurements();
    void reset();
    void performSelfTest();

    // For simulation
    void simulateFailure(bool simulate) { simulateFailure_ = simulate; }
    void setSimulationValues(const PowerReadings& values);

private:
    uint32_t readRegister(uint32_t regAddress);
    void writeRegister(uint32_t regAddress, uint32_t value);
    double convertToPower(uint32_t rawValue, double calibration) const;
    void validateRegisterValue(uint32_t regAddress, uint32_t value) const;

    I2CInterface* i2c_ = nullptr;
    uint8_t i2cAddress_ = 0x38;
    std::array<double, 3> calibrationFactors_;
    bool initialized_ = false;
    
    // Simulation members
    bool simulateFailure_ = false;
    PowerReadings simulatedValues_;
    mutable std::mutex mutex_;
};
