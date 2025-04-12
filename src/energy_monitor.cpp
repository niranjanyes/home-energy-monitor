// energy_monitor.cpp
#include "energy_monitor.h"
#include "i2c_interface.h"
#include "ade7878a.h"
#include "http_client.h"
#include "logger.h"
#include "config_manager.h"

class EnergyMonitor::Impl {
public:
    bool init() {
        try {
            // Load configuration
            if (!config.load("config.json")) {
                logger.logError("Failed to load configuration");
                return false;
            }

            // Initialize I2C
            if (!i2c.init(config.getI2CDevice())) {
                logger.logError("I2C initialization failed");
                return false;
            }

            // Initialize ADE7878A
            if (!ade.init(&i2c, config.getADECalibration())) {
                logger.logError("ADE7878A initialization failed");
                return false;
            }

            // Initialize HTTP client
            http.setBaseUrl(config.getServerUrl());
            http.setTimeout(config.getHttpTimeout());

            logger.logInfo("System initialized successfully");
            return true;
        } catch (const std::exception& e) {
            logger.logError(std::string("Initialization exception: ") + e.what());
            return false;
        }
    }

    void runMainLoop() {
        while (running) {
            try {
                auto readings = ade.readPowerMeasurements();
                if (validateReadings(readings)) {
                    cacheReadings(readings);
                    if (shouldSendToServer()) {
                        sendReadingsToServer();
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(config.getPollingInterval()));
            } catch (const std::exception& e) {
                logger.logError(std::string("Runtime exception: ") + e.what());
                handleRuntimeError();
            }
        }
    }

    // ... other private methods ...

private:
    I2CInterface i2c;
    ADE7878A ade;
    HttpClient http;
    Logger logger;
    ConfigManager config;
    bool running = false;
    std::vector<PowerReading> readingCache;
};

EnergyMonitor::EnergyMonitor() : pimpl(std::make_unique<Impl>()) {}
EnergyMonitor::~EnergyMonitor() = default;

bool EnergyMonitor::initialize() { return pimpl->init(); }
void EnergyMonitor::run() { pimpl->runMainLoop(); }
void EnergyMonitor::shutdown() { pimpl->shutdown(); }
