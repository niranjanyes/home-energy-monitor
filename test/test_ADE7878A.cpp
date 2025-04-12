#include "ade7878a.h"
#include "i2c_interface.h"
#include <iostream>

int main() {
    I2CInterface i2c;
    i2c.init("/dev/i2c-1");
    
    ADE7878A ade;
    std::array<double, 3> calib = {1.0, 1.0, 1.0};
    ade.init(&i2c, calib);
    
    for (int i = 0; i < 5; i++) {
        auto readings = ade.readPowerMeasurements();
        std::cout << "Sample " << i+1 << ":\n";
        std::cout << "  Voltage: " << readings.voltage << " V\n";
        for (size_t j = 0; j < 3; j++) {
            std::cout << "  Appliance " << j+1 << ": "
                      << readings.current[j] << " A, "
                      << readings.activePower[j] << " W\n";
        }
        std::cout << "  Frequency: " << readings.frequency << " Hz\n";
    }
    
    return 0;
}
