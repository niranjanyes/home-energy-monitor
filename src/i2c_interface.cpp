#include "i2c_interface.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdexcept>
#include <cstring>
#include "logger.h"

I2CInterface::I2CInterface() {
    Logger::getInstance().debug("I2CInterface constructor");
}

I2CInterface::~I2CInterface() {
    Logger::getInstance().debug("I2CInterface destructor");
}

void I2CInterface::initialize(const std::string& devicePath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized) {
        Logger::getInstance().warning("I2CInterface already initialized");
        return;
    }

    devicePath_ = devicePath;
    initialized = true;
    
    Logger::getInstance().info("I2CInterface initialized with device: " + devicePath);
}

void I2CInterface::checkInitialized() const {
    if (!initialized) {
        Logger::getInstance().error("I2CInterface not initialized");
        throw I2CException("I2C interface not initialized");
    }
}

void I2CInterface::checkAddress(uint8_t addr) const {
    if (addr < 0x03 || addr > 0x77) {
        Logger::getInstance().error("Invalid I2C address: " + std::to_string(addr));
        throw I2CException("Invalid I2C address");
    }
}

int I2CInterface::openDevice() const {
    int fd = open(devicePath_.c_str(), O_RDWR);
    if (fd < 0) {
        Logger::getInstance().error("Failed to open I2C device: " + std::string(strerror(errno)));
        throw I2CException("Failed to open I2C device");
    }
    return fd;
}

void I2CInterface::closeDevice(int fd) const {
    if (close(fd) < 0) {
        Logger::getInstance().error("Failed to close I2C device: " + std::string(strerror(errno)));
    }
}

void I2CInterface::writeByte(uint8_t addr, uint8_t reg, uint8_t value) {
    std::lock_guard<std::mutex> lock(mutex_);
    checkInitialized();
    checkAddress(addr);

    if (simulateFailure_) {
        Logger::getInstance().warning("Simulated I2C write failure");
        throw I2CException("Simulated write failure");
    }

    try {
        int fd = openDevice();
        
        if (ioctl(fd, I2C_SLAVE, addr) < 0) {
            closeDevice(fd);
            throw I2CException("Failed to set I2C slave address");
        }

        uint8_t buffer[2] = {reg, value};
        if (write(fd, buffer, 2) != 2) {
            closeDevice(fd);
            throw I2CException("Failed to write byte to I2C device");
        }

        closeDevice(fd);
        
        // For simulation, store the value
        simulatedRegisters_[addr][reg] = value;
        
        Logger::getInstance().debug("I2C write - addr: 0x" + 
            std::to_string(addr) + ", reg: 0x" + std::to_string(reg) + 
            ", value: 0x" + std::to_string(value));
    } catch (const I2CException& e) {
        Logger::getInstance().error(std::string("I2C write failed: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        Logger::getInstance().error(std::string("Unexpected error during I2C write: ") + e.what());
        throw I2CException(std::string("Unexpected error: ") + e.what());
    }
}

// ... (similar implementations for other I2CInterface methods) ...

void I2CInterface::setSimulatedRegister(uint8_t addr, uint8_t reg, uint8_t value) {
    std::lock_guard<std::mutex> lock(mutex_);
    simulatedRegisters_[addr][reg] = value;
    Logger::getInstance().debug("Set simulated register - addr: 0x" + 
        std::to_string(addr) + ", reg: 0x" + std::to_string(reg) + 
        ", value: 0x" + std::to_string(value));
}
