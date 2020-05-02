/**
 * \author argawaen
 * \date 30/04/2020
 *
 */
#include "I2CHelperFct.h"

namespace ob {

    PROGMEM constexpr uint8_t wireTransactionTimeout = 100;

    /// helper functions for I2C communications
    void write8(byte address, byte reg, byte value) {
        Wire.beginTransmission(address);
        Wire.write(reg);
        Wire.write(value);
        Wire.endTransmission();
    }

    bool requestFrom(byte address, byte reg, byte size) {
        Wire.beginTransmission(address);
        Wire.write(reg);
        Wire.endTransmission();
#ifdef I2CREQUESTCHECKS
        bool gotData = false;
        uint32_t start = millis(); // start timeout
        while (millis() - start < wireTransactionTimeout) {
            if (Wire.requestFrom(address, size) == size) {
                gotData = true;
                break;
            }
            delay(2);
        }
        return gotData;
#else
        Wire.requestFrom(address, size);
        return true;
#endif
    }

    uint32_t read(byte address, byte reg, byte size) {
        uint32_t value = 0;
        if (!requestFrom(address, reg, size))
            return 0; // error timeout
        for (byte i = 0; i < size; ++i)
            value = (value << 8u) | (uint8_t) Wire.read();
        Wire.endTransmission();
        return value;
    }

    byte read8(byte address, byte reg) {
        return read(address, reg, 1u);
    }

    uint16_t read16(byte address, byte reg) {
        return read(address, reg, 2u);
    }

    int16_t readS16(byte address, byte reg) {
        return read(address, reg, 2u);
    }
}