#include "http_client.h"
#include <sstream>
#include <thread>
#include "logger.h"

HttpClient::HttpClient() {
    Logger::getInstance().debug("HttpClient constructor");
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

HttpClient::~HttpClient() {
    if (curl_) {
        curl_easy_cleanup(curl_);
    }
    curl_global_cleanup();
    Logger::getInstance().debug("HttpClient destructor");
}

void HttpClient::initialize(const std::string& baseUrl, int timeoutMs) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized_) {
        Logger::getInstance().warning("HttpClient already initialized");
        return;
    }

    baseUrl_ = baseUrl;
    timeoutMs_ = timeoutMs;
    
    curl_ = curl_easy_init();
    if (!curl_) {
        Logger::getInstance().error("Failed to initialize CURL");
        throw NetworkException("Failed to initialize CURL");
    }

    initialized_ = true;
    Logger::getInstance().info("HttpClient initialized with base URL: " + baseUrl);
}

void HttpClient::setRetryPolicy(int maxRetries, int initialDelayMs, bool exponentialBackoff) {
    std::lock_guard<std::mutex> lock(mutex_);
    maxRetries_ = maxRetries;
    initialDelayMs_ = initialDelayMs;
    exponentialBackoff_ = exponentialBackoff;
    Logger::getInstance().debug("Set retry policy - max: " + std::to_string(maxRetries) + 
                              ", delay: " + std::to_string(initialDelayMs) + "ms");
}

json HttpClient::sendRequest(const std::string& endpoint, const json& data) {
    if (!initialized_) {
        Logger::getInstance().error("HttpClient not initialized");
        throw NetworkException("HTTP client not initialized");
    }

    if (simulateFailure_) {
        Logger::getInstance().warning("Simulated HTTP request failure");
        throw NetworkException("Simulated HTTP failure");
    }

    // Check for simulated response
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = simulatedResponses_.find(endpoint);
        if (it != simulatedResponses_.end()) {
            Logger::getInstance().debug("Returning simulated response for endpoint: " + endpoint);
            return it->second;
        }
    }

    std::string dataStr = data.dump();
    int attempt = 0;
    int delayMs = initialDelayMs_;

    while (attempt <= maxRetries_) {
        attempt++;
        try {
            std::string url = baseUrl_ + endpoint;
            Logger::getInstance().debug("HTTP attempt " + std::to_string(attempt) + 
                                      " to " + url);
            
            json response = performHttpRequest(url, dataStr);
            return response;
        } catch (const NetworkException& e) {
            if (attempt >= maxRetries_) {
                Logger::getInstance().error("HTTP request failed after " + 
                                          std::to_string(maxRetries_) + " attempts: " + 
                                          e.what());
                throw;
            }
            
            Logger::getInstance().warning("HTTP request failed (attempt " + 
                                        std::to_string(attempt) + "): " + e.what());
            waitForRetry(attempt);
            
            if (exponentialBackoff_) {
                delayMs *= 2;
            }
        }
    }

    throw NetworkException("Unexpected state in HTTP request");
}

// ... (implementations of other HttpClient methods) ...

void HttpClient::setSimulatedResponse(const std::string& endpoint, const json& response) {
    std::lock_guard<std::mutex> lock(mutex_);
    simulatedResponses_[endpoint] = response;
    Logger::getInstance().debug("Set simulated response for endpoint: " + endpoint);
}
