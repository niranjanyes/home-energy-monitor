#!/bin/bash

# Install dependencies on Ubuntu/Debian
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    libcurl4-openssl-dev \
    git \
    python3 \
    python3-pip \
    i2c-tools \
    libi2c-dev

# Install mock server dependencies
pip3 install flask flask-cors

# Clone and build nlohmann/json (handled by CMake)
mkdir -p build
cd build
cmake ..
make
