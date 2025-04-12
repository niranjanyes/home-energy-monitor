#include "logger.h"
#include <filesystem>
#include <iostream>

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::initialize(const std::string& logFilePath, LogLevel level) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    try {
        // Create directory if it doesn't exist
        auto dir = std::filesystem::path(logFilePath).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }

        logFile.open(logFilePath, std::ios::out | std::ios::app);
        if (!logFile.is_open()) {
            throw std::runtime_error("Failed to open log file: " + logFilePath);
        }

        currentLevel = level;
        initialized = true;
        
        logFile << "\n\n=== Logging started at " << getCurrentTimestamp() << " ===\n";
        info("Logger initialized with level: " + levelToString(level));
    } catch (const std::exception& e) {
        std::cerr << "Logger initialization failed: " << e.what() << std::endl;
        throw ConfigException("Logger initialization failed: " + std::string(e.what()));
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    if (!initialized || level < currentLevel) return;

    std::lock_guard<std::mutex> lock(logMutex);
    try {
        logFile << "[" << getCurrentTimestamp() << "] "
                << "[" << levelToString(level) << "] "
                << message << std::endl;
        
        // Also echo errors and critical messages to stderr
        if (level >= LogLevel::ERROR) {
            std::cerr << "[" << levelToString(level) << "] " << message << std::endl;
        }
    } catch (...) {
        // If logging fails, we can't really log that fact, so just print to stderr
        std::cerr << "Logging failed for message: " << message << std::endl;
    }
}

std::string Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

// Convenience methods
void Logger::debug(const std::string& message) { log(LogLevel::DEBUG, message); }
void Logger::info(const std::string& message) { log(LogLevel::INFO, message); }
void Logger::warning(const std::string& message) { log(LogLevel::WARNING, message); }
void Logger::error(const std::string& message) { log(LogLevel::ERROR, message); }
void Logger::critical(const std::string& message) { log(LogLevel::CRITICAL, message); }
