#include "ade7878a.h"
#include <cmath>
#include <thread>
#include <random>
#include "logger.h"

ADE7878A::ADE7878A() {
    Logger::getInstance().debug("ADE7878A constructor");
}

ADE7878A::~ADE7878A() {
    Logger::getInstance().debug("ADE7878A destructor");
}

void ADE7878A::initialize(I2CInterface* i2c, const std::array<double, 3>& calibrationFactors) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized_) {
        Logger::getInstance().warning("ADE7878A already initialized");
        return;
    }

    if (!i2c || !i2c->isInitialized()) {
        Logger::getInstance().error("Invalid I2C interface provided");
        throw ADE7878AException("Invalid I2C interface");
    }

    i2c_ = i2c;
    calibrationFactors_ = calibrationFactors;
    
    try {
        // Perform device reset
        reset();
        
        // Verify communication by reading a known register
        uint32_t deviceId = readRegister(0xE7FE); // ADE7878A device ID register
        if ((deviceId >> 16) != 0x7878) {
            throw ADE7878AException("Device ID verification failed");
        }
        
        // Configure the device
        writeRegister(0xE7FE, 0x0030); // Configure measurement mode
        writeRegister(0xE70E, 0x0040); // Enable voltage sag detection
        
        initialized_ = true;
        
        Logger::getInstance().info("ADE7878A initialized successfully");
    } catch (const ADE7878AException& e) {
        Logger::getInstance().error(std::string("ADE7878A initialization failed: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        Logger::getInstance().error(std::string("Unexpected error during ADE7878A initialization: ") + e.what());
        throw ADE7878AException(std::string("Unexpected error: ") + e.what());
    }
}

PowerReadings ADE7878A::readPowerMeasurements() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!initialized_) {
        Logger::getInstance().error("ADE7878A not initialized");
        throw ADE7878AException("Device not initialized");
    }

    if (simulateFailure_) {
        Logger::getInstance().warning("Simulated ADE7878A read failure");
        throw ADE7878AException("Simulated read failure");
    }

    try {
        PowerReadings readings;
        
        // Get timestamp
        auto now = std::chrono::system_clock::now();
        readings.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            now.time_since_epoch()).count();
        
        // Read voltage (register 0x31C0 - AVRMS)
        uint32_t voltageRaw = readRegister(0x31C0);
        readings.voltage = convertToPower(voltageRaw, 1.0);
        
        // Read currents (registers 0x31C1-0x31C3 - AIRMS for phases A, B, C)
        readings.current[0] = convertToPower(readRegister(0x31C1), calibrationFactors_[0]);
        readings.current[1] = convertToPower(readRegister(0x31C2), calibrationFactors_[1]);
        readings.current[2] = convertToPower(readRegister(0x31C3), calibrationFactors_[2]);
        
        // Read active powers (registers 0x31C4-0x31C6 - AWATT for phases A, B, C)
        readings.activePower[0] = convertToPower(readRegister(0x31C4), calibrationFactors_[0]);
        readings.activePower[1] = convertToPower(readRegister(0x31C5), calibrationFactors_[1]);
        readings.activePower[2] = convertToPower(readRegister(0x31C6), calibrationFactors_[2]);
        
        // Read reactive powers (registers 0x31C7-0x31C9 - AVAR for phases A, B, C)
        readings.reactivePower[0] = convertToPower(readRegister(0x31C7), calibrationFactors_[0]);
        readings.reactivePower[1] = convertToPower(readRegister(0x31C8), calibrationFactors_[1]);
        readings.reactivePower[2] = convertToPower(readRegister(0x31C9), calibrationFactors_[2]);
        
        // Calculate apparent power
        for (int i = 0; i < 3; i++) {
            readings.apparentPower[i] = sqrt(
                readings.activePower[i] * readings.activePower[i] + 
                readings.reactivePower[i] * readings.reactivePower[i]);
        }
        
        // Read frequency (register 0x31CA - FREQ)
        uint32_t freqRaw = readRegister(0x31CA);
        readings.frequency = static_cast<double>(freqRaw) / 100.0;
        
        Logger::getInstance().debug("Read power measurements - V: " + 
            std::to_string(readings.voltage) + ", I: [" +
            std::to_string(readings.current[0]) + ", " +
            std::to_string(readings.current[1]) + ", " +
            std::to_string(readings.current[2]) + "]");
            
        return readings;
    } catch (const ADE7878AException& e) {
        Logger::getInstance().error(std::string("Failed to read power measurements: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        Logger::getInstance().error(std::string("Unexpected error during power measurement: ") + e.what());
        throw ADE7878AException(std::string("Unexpected error: ") + e.what());
    }
}

// ... (implementations of other ADE7878A methods) ...

void ADE7878A::setSimulationValues(const PowerReadings& values) {
    std::lock_guard<std::mutex> lock(mutex_);
    simulatedValues_ = values;
    Logger::getInstance().debug("Set ADE7878A simulated values");
}
