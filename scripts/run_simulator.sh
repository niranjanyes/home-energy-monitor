#!/bin/bash

# Load the i2c-dev module
sudo modprobe i2c-dev

# Create virtual I2C bus
sudo i2c_add_virtual_device 1 0x38  # ADE7878A address

echo "Virtual I2C bus created at /dev/i2c-1 with device at 0x38"
