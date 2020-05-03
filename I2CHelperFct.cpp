/**
 * \author argawaen
 * \date 30/04/2020
 *
 */
#include "I2CHelperFct.h"
#include <Wire.h>

namespace ob {

    void wireInit(){
        Wire.begin();
    }

    /// helper functions for I2C communications
    void write8(u8 address, u8 reg, u8 value) {
        Wire.beginTransmission(address);
        Wire.write(reg);
        Wire.write(value);
        Wire.endTransmission();
    }

    bool requestFrom(u8 address, u8 reg, u8 size) {
        Wire.beginTransmission(address);
        Wire.write(reg);
        Wire.endTransmission();
#ifdef I2CREQUESTCHECKS
        bool gotData = false;
        u32 start = millis(); // start timeout
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

    s32 read(u8 address, u8 reg, u8 size, bool msb) {
        s32 value = 0;
        if (!requestFrom(address, reg, size))
            return 0; // error timeout
        for (u8 i = 0; i < size; ++i)
            if (msb)
                value = (value << 8u) | Wire.read();
            else
                value |= Wire.read() << (i * 8u) ;
        Wire.endTransmission();
        return value;
    }


    s8 read8(u8 address, u8 reg) {
        return read(address, reg, 1u, true);
    }

    s16 read16Msb(u8 address, u8 reg) {
        return read(address, reg, 2u, true);
    }
    s16 read16Lsb(u8 address, u8 reg) {
        return read(address, reg, 2u, false);
    }

}