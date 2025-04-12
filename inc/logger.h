#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <memory>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger {
public:
    static Logger& getInstance();
    
    void initialize(const std::string& logFilePath, LogLevel level = LogLevel::INFO);
    void log(LogLevel level, const std::string& message);
    
    // Convenience methods
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);

    // Delete copy/move constructors and assignment operators
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

private:
    Logger() = default;
    ~Logger();

    std::string getCurrentTimestamp() const;
    std::string levelToString(LogLevel level) const;
    
    std::ofstream logFile;
    LogLevel currentLevel = LogLevel::INFO;
    std::mutex logMutex;
    bool initialized = false;
};
