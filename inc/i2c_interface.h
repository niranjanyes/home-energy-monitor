#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <mutex>
#include "exceptions.h"

class I2CInterface {
public:
    I2CInterface();
    ~I2CInterface();

    void initialize(const std::string& devicePath);
    bool isInitialized() const { return initialized; }

    void writeByte(uint8_t addr, uint8_t reg, uint8_t value);
    void writeBlock(uint8_t addr, uint8_t reg, const std::vector<uint8_t>& data);
    
    uint8_t readByte(uint8_t addr, uint8_t reg);
    std::vector<uint8_t> readBlock(uint8_t addr, uint8_t reg, size_t length);

    // For simulation
    void simulateFailure(bool simulate) { simulateFailure_ = simulate; }
    void setSimulatedRegister(uint8_t addr, uint8_t reg, uint8_t value);

private:
    void checkInitialized() const;
    void checkAddress(uint8_t addr) const;
    int openDevice() const;
    void closeDevice(int fd) const;

    std::string devicePath_;
    bool initialized = false;
    mutable std::mutex mutex_;
    
    // Simulation members
    bool simulateFailure_ = false;
    std::unordered_map<uint8_t, std::unordered_map<uint8_t, uint8_t>> simulatedRegisters_;
};
