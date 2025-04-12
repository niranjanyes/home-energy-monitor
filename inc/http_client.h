#pragma once

#include <string>
#include <curl/curl.h>
#include <memory>
#include <mutex>
#include "nlohmann/json.hpp"
#include "exceptions.h"

using json = nlohmann::json;

class HttpClient {
public:
    HttpClient();
    ~HttpClient();

    void initialize(const std::string& baseUrl, int timeoutMs = 5000);
    bool isInitialized() const { return initialized_; }

    void setRetryPolicy(int maxRetries, int initialDelayMs, bool exponentialBackoff = true);
    void setAuthentication(const std::string& apiKey);

    json sendRequest(const std::string& endpoint, const json& data);
    bool testConnection();

    // For simulation
    void simulateFailure(bool simulate) { simulateFailure_ = simulate; }
    void setSimulatedResponse(const std::string& endpoint, const json& response);

private:
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output);
    json performHttpRequest(const std::string& url, const std::string& data);
    void handleCurlError(CURLcode res, const std::string& context);
    void waitForRetry(int attempt) const;

    std::string baseUrl_;
    int timeoutMs_;
    int maxRetries_ = 3;
    int initialDelayMs_ = 1000;
    bool exponentialBackoff_ = true;
    std::string apiKey_;
    CURL* curl_ = nullptr;
    bool initialized_ = false;
    
    // Simulation members
    bool simulateFailure_ = false;
    std::unordered_map<std::string, json> simulatedResponses_;
    mutable std::mutex mutex_;
};
