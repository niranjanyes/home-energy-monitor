// config_manager.h
#pragma once

#include <string>
#include <array>
#include "json.hpp"

using json = nlohmann::json;

class ConfigManager {
public:
    bool load(const std::string& configPath);
    void save(const std::string& configPath);

    // Getters
    std::string getI2CDevice() const;
    std::string getServerUrl() const;
    int getPollingInterval() const;
    int getHttpTimeout() const;
    std::array<double, 3> getADECalibration() const;
    int getMaxCacheSize() const;

    // Setters
    void setI2CDevice(const std::string& device);
    void setServerUrl(const std::string& url);
    // ... other setters ...

private:
    json configJson;
    const json defaultConfig = {
        {"i2c", {
            {"device", "/dev/i2c-1"},
            {"ade7878a_address", 0x38}
        }},
        {"server", {
            {"url", "http://localhost:3000/api/readings"},
            {"timeout_ms", 5000},
            {"max_retries", 3}
        }},
        {"polling", {
            {"interval_ms", 1000},
            {"max_cache_size", 100}
        }},
        {"calibration", {
            {"appliance1", 1.0},
            {"appliance2", 1.0},
            {"appliance3", 1.0}
        }}
    };
};
