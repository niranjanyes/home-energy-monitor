#include "energy_monitor.h"
#include "logger.h"
#include <csignal>
#include <cstdlib>
#include <iostream>

std::atomic<bool> running(true);

void signalHandler(int signal) {
    Logger::getInstance().info("Received signal " + std::to_string(signal) + ", shutting down");
    running = false;
}

int main(int argc, char* argv[]) {
    try {
        // Set up signal handling
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
        // Initialize logger
        Logger::getInstance().initialize("logs/energy_monitor.log", LogLevel::DEBUG);
        Logger::getInstance().info("Starting Energy Monitoring System");
        
        // Check command line arguments
        if (argc < 2) {
            Logger::getInstance().critical("Configuration file path not provided");
            std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
            return EXIT_FAILURE;
        }
        
        // Create and initialize energy monitor
        EnergyMonitor monitor;
        if (!monitor.initialize(argv[1])) {
            Logger::getInstance().critical("Energy monitor initialization failed");
            return EXIT_FAILURE;
        }
        
        // Main loop
        while (running) {
            try {
                monitor.run();
            } catch (const std::exception& e) {
                Logger::getInstance().error(std::string("Error in main loop: ") + e.what());
                
                // Implement recovery strategy
                if (!monitor.recover()) {
                    Logger::getInstance().critical("Recovery failed, shutting down");
                    break;
                }
            }
        }
        
        // Clean shutdown
        monitor.shutdown();
        Logger::getInstance().info("Energy Monitoring System stopped gracefully");
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        Logger::getInstance().critical(std::string("Fatal error: ") + e.what());
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
