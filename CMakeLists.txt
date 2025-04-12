cmake_minimum_required(VERSION 3.12)
project(EnergyMonitor VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

# JSON library (nlohmann/json)
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# Main executable
add_executable(energy_monitor
    src/main.cpp
    src/energy_monitor.cpp
    src/i2c_interface.cpp
    src/ade7878a.cpp
    src/http_client.cpp
    src/logger.cpp
    src/config_manager.cpp
)

target_include_directories(energy_monitor PRIVATE include)
target_link_libraries(energy_monitor PRIVATE 
    ${CURL_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    nlohmann_json::nlohmann_json
)

# Tests
enable_testing()
add_subdirectory(test)
