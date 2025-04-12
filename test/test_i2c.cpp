#include "i2c_interface.h"
#include <cassert>
#include <iostream>

int main() {
    I2CInterface i2c;
    assert(i2c.init("/dev/i2c-1"));
    
    // Test write/read
    const uint8_t testAddr = 0x38;
    const uint8_t testReg = 0x10;
    const uint8_t testValue = 0xAB;
    
    assert(i2c.writeByte(testAddr, testReg, testValue));
    assert(i2c.readByte(testAddr, testReg) == testValue);
    
    std::cout << "I2C interface test passed!" << std::endl;
    return 0;
}
