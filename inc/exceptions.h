#pragma once

#include <stdexcept>
#include <string>

class EnergyMonitorException : public std::runtime_error {
public:
    explicit EnergyMonitorException(const std::string& msg)
        : std::runtime_error(msg) {}
};

class I2CException : public EnergyMonitorException {
public:
    explicit I2CException(const std::string& msg)
        : EnergyMonitorException("I2C Error: " + msg) {}
};

class ADE7878AException : public EnergyMonitorException {
public:
    explicit ADE7878AException(const std::string& msg)
        : EnergyMonitorException("ADE7878A Error: " + msg) {}
};

class ConfigException : public EnergyMonitorException {
public:
    explicit ConfigException(const std::string& msg)
        : EnergyMonitorException("Configuration Error: " + msg) {}
};

class NetworkException : public EnergyMonitorException {
public:
    explicit NetworkException(const std::string& msg)
        : EnergyMonitorException("Network Error: " + msg) {}
};

class DataValidationException : public EnergyMonitorException {
public:
    explicit DataValidationException(const std::string& msg)
        : EnergyMonitorException("Data Validation Error: " + msg) {}
};
