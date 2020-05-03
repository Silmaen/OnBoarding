/**
 * \author argawaen
 * \date 10/04/2020
 *
 */
#include "I2CHelperFct.h"
#include "systemClock.h"

#ifndef NO_CLOCK

// i2c slave address of the DS3231 chip
PROGMEM constexpr uint8_t ds3231I2CAddr = 0x68;
// timekeeping registers
PROGMEM constexpr uint8_t ds3231TimeCalAddr = 0x00;
PROGMEM constexpr uint8_t ds3231ControlAddr = 0x0E;
PROGMEM constexpr uint8_t ds3231StatusAddr = 0x0F;
PROGMEM constexpr uint8_t ds3231TemperatureAddr = 0x11;
// control register bits
PROGMEM constexpr uint8_t ds3231ControlIntcn = 0x4;    ///< Interrupt Control */
// status register bits
PROGMEM constexpr uint8_t ds3231StatusEn32Khz = 0x08;    ///< Enable 32KHz Output
#endif
PROGMEM constexpr uint8_t corr = ~((1u << 8u) - 1u);
namespace ob::time {

#ifndef NO_CLOCK

    uint8_t dec2Bcd(const uint8_t val) { return ((val / 10 * 16) + (val % 10)); }

    uint8_t bcd2Dec(const uint8_t val) { return ((val / 16 * 10) + (val % 16)); }

// temperature register
    float getClockTemperature() {
        uint8_t tempMsb = read(ds3231I2CAddr, ds3231TemperatureAddr, 1u);
        if ((tempMsb & 0x80u) != 0u)
            return 0.25f * (read(ds3231I2CAddr, ds3231TemperatureAddr+1, 1u) >> 6u) + (tempMsb | corr);
        else
            return 0.25f * (read(ds3231I2CAddr, ds3231TemperatureAddr+1, 1u) >> 6u) + tempMsb;
    }

#endif

    void setup() {
#ifndef NO_CLOCK
        wireInit();
        write8(ds3231I2CAddr, ds3231ControlAddr, ds3231ControlIntcn);
        write8(ds3231I2CAddr, ds3231StatusAddr, read8(ds3231I2CAddr, ds3231StatusAddr) & ~ds3231StatusEn32Khz);
#endif
    }

    void updateFromDevice(uint16_t &year, uint8_t &month, uint8_t &day, uint8_t &hour, uint8_t &minute, uint8_t &second) {
#ifndef NO_CLOCK
        second = bcd2Dec(read8(ds3231I2CAddr, ds3231TimeCalAddr));
        minute = bcd2Dec(read8(ds3231I2CAddr, ds3231TimeCalAddr+1));
        hour = bcd2Dec(read8(ds3231I2CAddr, ds3231TimeCalAddr+2));
        day = bcd2Dec(read8(ds3231I2CAddr, ds3231TimeCalAddr+4));
        uint8_t n = read8(ds3231I2CAddr, ds3231TimeCalAddr+5);
        month = bcd2Dec(n & 0x1Fu);
        uint8_t century = (n & 0x80u) >> 7u;
        year = 1900u + bcd2Dec(read8(ds3231I2CAddr, ds3231TimeCalAddr+6)) + 100u * (century == 1u);
#else
        second = 0;
        minute = 0;
        hour = 0;
        day = 1;
        month = 1;
        year = 1970u;
#endif
    }

    void setDateTime(const uint16_t &year, const uint8_t &month, const uint8_t &day, const uint8_t &hour,
                     const uint8_t &minute, const uint8_t &second) {
#ifndef NO_CLOCK
        // update the device time with internal one
        uint8_t century, yearS;
        if (year >= 2000) {
            century = 0x80;
            yearS = year - 2000;
        } else {
            century = 0;
            yearS = year - 1900;
        }
        write8(ds3231I2CAddr,ds3231TimeCalAddr,dec2Bcd(second));
        write8(ds3231I2CAddr,ds3231TimeCalAddr+1,dec2Bcd(minute));
        write8(ds3231I2CAddr,ds3231TimeCalAddr+2,dec2Bcd(hour));
        write8(ds3231I2CAddr,ds3231TimeCalAddr+4,dec2Bcd(day));
        write8(ds3231I2CAddr,ds3231TimeCalAddr+5,dec2Bcd(month)+century);
        write8(ds3231I2CAddr,ds3231TimeCalAddr+6,dec2Bcd(yearS));
#endif
    }

    static void twoDigitIntc(const uint8_t &i, char *res) {
        res[0] = '0' + i / 10;
        res[1] = '0' + i % 10;
    }

    static void fourDigitIntc(const uint16_t &i, char *res) {
        res[0] = '0' + i / 1000;
        res[1] = '0' + i % 1000 / 100;
        res[2] = '0' + i % 100 / 10;
        res[3] = '0' + i % 10;
    }

    void getTimeStamp(char *res) {
        uint16_t year;
        uint8_t month, day, hour, minute, second;
        updateFromDevice(year, month, day, hour, minute, second);
        res[0] = '[';
        fourDigitIntc(year, &res[1]); // 1 2 3 4
        res[5] = '-';
        twoDigitIntc(month, &res[6]); // 6 7
        res[8] = '-';
        twoDigitIntc(day, &res[9]); // 9 10
        res[11] = ' ';
        twoDigitIntc(hour, &res[12]); // 12 13
        res[14] = ':';
        twoDigitIntc(minute, &res[15]); // 15 16
        res[17] = ':';
        twoDigitIntc(second, &res[18]); // 18 19
        res[20] = ']';
        res[21] = '\0';
    }
}

