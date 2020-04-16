/**
 * \author argawaen
 * \date 10/04/2020
 *
 */
#include <Wire.h>
#include "systemClock.h"

template<> ob::core::systemClock ob::baseManager<ob::core::systemClock>::instance{ob::core::systemClock()};


#ifndef NO_CLOCK
PROGMEM constexpr uint8_t ds3231TransactionTimeout = 100;   ///< Wire NAK/Busy timeout in ms

PROGMEM constexpr uint32_t secondsFrom1970To2000 = 946684800;

// i2c slave address of the DS3231 chip
PROGMEM constexpr uint8_t ds3231I2CAddr = 0x68;

// timekeeping registers
PROGMEM constexpr uint8_t ds3231TimeCalAddr = 0x00;
PROGMEM constexpr uint8_t ds3231Alarm1Addr = 0x07;
PROGMEM constexpr uint8_t ds3231Alarm2Addr = 0x0B;
PROGMEM constexpr uint8_t ds3231ControlAddr = 0x0E;
PROGMEM constexpr uint8_t ds3231StatusAddr = 0x0F;
PROGMEM constexpr uint8_t ds3231AgingOffsetAddr = 0x10;
PROGMEM constexpr uint8_t ds3231TemperatureAddr = 0x11;

// control register bits
PROGMEM constexpr uint8_t ds3231ControlA1Ie = 0x1;    ///< Alarm 2 Interrupt Enable
PROGMEM constexpr uint8_t ds3231ControlA2Ie = 0x2;    ///< Alarm 2 Interrupt Enable
PROGMEM constexpr uint8_t ds3231ControlIntcn = 0x4;    ///< Interrupt Control */
PROGMEM constexpr uint8_t ds3231ControlRs1 = 0x8;    ///< square-wave rate select 2
PROGMEM constexpr uint8_t ds3231ControlRs2 = 0x10;    ///< square-wave rate select 2
PROGMEM constexpr uint8_t ds3231ControlConv = 0x20;    ///< Convert Temperature
PROGMEM constexpr uint8_t ds3231ControlBbsqw = 0x40;    ///< Battery-Backed Square-Wave Enable
PROGMEM constexpr uint8_t ds3231ControlEosc = 0x80;    ///< not Enable Oscillator, 0 equal on


// status register bits
PROGMEM constexpr uint8_t ds3231StatusA1F = 0x01;    ///< Alarm 1 Flag
PROGMEM constexpr uint8_t ds3231StatusA2F = 0x02;    ///< Alarm 2 Flag
PROGMEM constexpr uint8_t ds3231StatusBusy = 0x04;    ///< device is busy executing TCXO
PROGMEM constexpr uint8_t ds3231StatusEn32Khz = 0x08;    ///< Enable 32KHz Output
PROGMEM constexpr uint8_t ds3231StatusOsf = 0x80;    ///< Oscillator Stop Flag
#endif
namespace ob::core {

#ifndef NO_CLOCK
#ifdef __AVR__

#ifdef CONFIG_UNIX_TIME
#include <avr/pgmspace.h>
#endif

    // Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif
#else
#define PROGMEM
#define xpgm_read_byte(addr) (*(const uint8_t *)(addr))
#endif

#if defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_ESP32)
    // on the the D21 and ESP32 the C-Runtime uses already UNIX time
#define CONFIG_UNIX_TIME
#else
// comment this out if you need unixtime support
// this will add about 324bytes to your firmware
//#define CONFIG_UNIX_TIME
#endif
#ifdef USE_HELPER
    struct ts {
        uint8_t sec = 0;         ///< seconds
        uint8_t min = 0;         ///< minutes
        uint8_t hour = 0;        ///< hours
        uint8_t mday = 0;        ///< day of the month
        uint8_t mon = 0;         ///< month
        int16_t year = 0;        ///< year
        uint8_t wday = 0;        ///< day of the week
        uint8_t yday = 0;        ///< day in the year
        uint8_t isdst = 0;       ///< daylight saving time
        uint8_t yearS = 0;       ///< year in short notation
#ifdef CONFIG_UNIX_TIME
        uint32_t unixtime=0;   ///< seconds since 01.01.1970 00:00:00 UTC
#endif
    };
#endif

    uint8_t dec2Bcd(const uint8_t val) { return ((val / 10 * 16) + (val % 10)); }

    uint8_t bcd2Dec(const uint8_t val) { return ((val / 16 * 10) + (val % 16)); }

#ifdef INP2TOI
    uint8_t inp2Toi(const char *cmd, const uint16_t seek){
        uint8_t rv;
        rv = (cmd[seek] - 48) * 10 + cmd[seek + 1] - 48;
        return rv;
    }
#endif

    void setAddr(const uint8_t addr, const uint8_t val) {
        Wire.beginTransmission(ds3231I2CAddr);
        Wire.write(addr);
        Wire.write(val);
        Wire.endTransmission();
    }

    uint8_t getAddr(const uint8_t addr) {
        uint8_t rv;
        Wire.beginTransmission(ds3231I2CAddr);
        Wire.write(addr);
        Wire.endTransmission();
        bool gotData = false;
        uint32_t start = millis(); // start timeout
        while (millis() - start < ds3231TransactionTimeout) {
            if (Wire.requestFrom(ds3231I2CAddr, 1u) == 1) {
                gotData = true;
                break;
            }
            delay(2);
        }
        if (!gotData)
            return 0; // error timeout
        rv = Wire.read();
        return rv;
    }

    // control register
/* control register 0Eh/8Eh
bit7 EOSC   Enable Oscillator (1 if oscillator must be stopped when on battery)
bit6 BBSQW  Battery Backed Square Wave
bit5 CONV   Convert temperature (1 forces a conversion NOW)
bit4 RS2    Rate select - frequency of square wave output
bit3 RS1    Rate select
bit2 INTCN  Interrupt control (1 for use of the alarms and to disable square wave)
bit1 A2IE   Alarm2 interrupt enable (1 to enable)
bit0 A1IE   Alarm1 interrupt enable (1 to enable)
*/
    void setCreg(const uint8_t val) {
        setAddr(ds3231ControlAddr, val);
    }

    uint8_t getCreg() {
        uint8_t rv;
        rv = getAddr(ds3231ControlAddr);
        return rv;
    }

    // status register 0Fh/8Fh

/*
bit7 OSF      Oscillator Stop Flag (if 1 then oscillator has stopped and date might be innacurate)
bit3 EN32kHz  Enable 32kHz output (1 if needed)
bit2 BSY      Busy with TCXO functions
bit1 A2F      Alarm 2 Flag - (1 if alarm2 was triggered)
bit0 A1F      Alarm 1 Flag - (1 if alarm1 was triggered)
*/
    void setSreg(const uint8_t val) {
        setAddr(ds3231StatusAddr, val);
    }

    uint8_t getSreg() {
        uint8_t rv;
        rv = getAddr(ds3231StatusAddr);
        return rv;
    }

#ifdef AGING_REGISTER
    // aging register
    void setAging(const int8_t val) {
        uint8_t reg;
        if (val >= 0)
            reg = val;
        else
            reg = ~(-val) + 1u;      // 2C
        /*
         * At 25°C the aging change of:
         * +1 means -0.1ppm
         * -1 means -0.1ppm
         */
        setAddr(ds3231AgingOffsetAddr, reg);
        /*
         * A conversion mut be done to forace the new aging value.
         */
        setCreg(getCreg() | ds3231ControlConv);
    }

    int8_t getAging() {
        uint8_t reg;
        int8_t rv;
        reg = getAddr(ds3231AgingOffsetAddr);
        if ((reg & 0x80u) != 0u)
            rv = reg | ~((1u << 8u) - 1u);     // if negative getTime two's complement
        else
            rv = reg;
        return rv;
    }
#endif

// temperature register

    float getTreg() {
        float rv;
        uint8_t tempMsb, tempLsb;
        int8_t nint;
        Wire.beginTransmission(ds3231I2CAddr);
        Wire.write(ds3231TemperatureAddr);
        Wire.endTransmission();
        bool gotData = false;
        uint32_t start = millis(); // start timeout
        while (millis() - start < ds3231TransactionTimeout) {
            if (Wire.requestFrom(ds3231I2CAddr, 2u) == 2) {
                gotData = true;
                break;
            }
            delay(2);
        }
        if (!gotData)
            return 0; // error timeout
        tempMsb = Wire.read();
        tempLsb = Wire.read() >> 6u;
        if ((tempMsb & 0x80u) != 0u)
            nint = tempMsb | ~((1u << 8u) - 1u);      // if negative getTime two's complement
        else
            nint = tempMsb;
        rv = 0.25f * tempLsb + nint;
        return rv;
    }

    void set32KHzOutput(const uint8_t on) {
        /*
         * Note, the pin1 is an open drain pin, therfore a pullup
         * resitor is required to use the output.
         */
        if (on) {
            uint8_t sreg = getSreg();
            sreg &= ~ds3231StatusOsf;
            sreg |= ds3231StatusEn32Khz;
            setSreg(sreg);
        } else {
            uint8_t sreg = getSreg();
            sreg &= ~ds3231StatusEn32Khz;
            setSreg(sreg);
        }
    }

// alarms

#ifdef ALARMS
    // flags are: A1M1 (seconds), A1M2 (minutes), A1M3 (hour),
    // A1M4 (day) 0 to enable, 1 to disable, DY/DT (dayofweek == 1/dayofmonth == 0)
        void setA1(const uint8_t s, const uint8_t mi, const uint8_t h, const uint8_t d, const uint8_t *flags) {
            uint8_t t[4] = {s, mi, h, d};
            uint8_t i;
            Wire.beginTransmission(ds3231I2CAddr);
            Wire.write(ds3231Alarm1Addr);
            for (i = 0; i <= 3; i++) {
                if (i == 3) {
                    Wire.write(dec2Bcd(t[3]) | (flags[3] << 7u) | (flags[4] << 6u));
                } else
                    Wire.write(dec2Bcd(t[i]) | (flags[i] << 7u));
            }
            Wire.endTransmission();
        }

        void getA1(char *buf, const uint8_t len) {
            uint8_t n[4];
            uint8_t t[4];               //second,minute,hour,day
            uint8_t f[5];               // flags
            uint8_t i;
            Wire.beginTransmission(ds3231I2CAddr);
            Wire.write(ds3231Alarm1Addr);
            Wire.endTransmission();
            uint8_t gotData = false;
            uint32_t start = millis(); // start timeout
            while (millis() - start < ds3231TransactionTimeout) {
                if (Wire.requestFrom(ds3231I2CAddr, 4u) == 4) {
                    gotData = true;
                    break;
                }
                delay(2);
            }
            if (!gotData)
                return; // error timeout
            for (i = 0; i <= 3; i++) {
                n[i] = Wire.read();
                f[i] = (n[i] & 0x80u) >> 7u;
                t[i] = bcd2Dec(n[i] & 0x7F);
            }
            f[4] = (n[3] & 0x40u) >> 6u;
            t[3] = bcd2Dec(n[3] & 0x3Fu);
            snprintf(buf, len,
                     "s%02d m%02d h%02d d%02d fs%d m%d h%d d%d wm%d %d %d %d %d",
                     t[0], t[1], t[2], t[3], f[0], f[1], f[2], f[3], f[4], n[0],
                     n[1], n[2], n[3]);

        }

    // when the alarm flag is cleared the pulldown on INT is also released
        void clearA1F() {
            uint8_t regVal;
            regVal = getSreg() & ~ds3231StatusA1F;
            setSreg(regVal);
        }
        uint8_t triggeredA1(){
            return getSreg() & ds3231StatusA1F;
        }


    // flags are: A2M2(minutes), A2M3 (hour), A2M4 (day) 0 to enable, 1 to disable, DY/DT (dayofweek==1/dayofmonth==0)-
        void setA2(const uint8_t mi, const uint8_t h, const uint8_t d, const uint8_t *flags) {
            uint8_t t[3] = {mi, h, d};
            uint8_t i;
            Wire.beginTransmission(ds3231I2CAddr);
            Wire.write(ds3231Alarm2Addr);
            for (i = 0; i <= 2; i++) {
                if (i == 2) {
                    Wire.write(dec2Bcd(t[2]) | (flags[2] << 7u) | (flags[3] << 6u));
                } else
                    Wire.write(dec2Bcd(t[i]) | (flags[i] << 7u));
            }
            Wire.endTransmission();
        }

        void getA2(char *buf, const uint8_t len) {
            uint8_t n[3];
            uint8_t t[3];               //second,minute,hour,day
            uint8_t f[4];               // flags
            uint8_t i;
            Wire.beginTransmission(ds3231I2CAddr);
            Wire.write(ds3231Alarm2Addr);
            Wire.endTransmission();
            Wire.requestFrom(ds3231I2CAddr, 4u);
            uint8_t gotData = false;
            uint32_t start = millis(); // start timeout
            while (millis() - start < ds3231TransactionTimeout) {
                if (Wire.requestFrom(ds3231I2CAddr, 3u) == 3) {
                    gotData = true;
                    break;
                }
                delay(2);
            }
            if (!gotData)
                return; // error timeout
            for (i = 0; i <= 2; i++) {
                n[i] = Wire.read();
                f[i] = (n[i] & 0x80u) >> 7u;
                t[i] = bcd2Dec(n[i] & 0x7F);
            }
            f[3] = (n[2] & 0x40u) >> 6u;
            t[2] = bcd2Dec(n[2] & 0x3F);
            snprintf(buf, len, "m%02d h%02d d%02d fm%d h%d d%d wm%d %d %d %d", t[0],
                     t[1], t[2], f[0], f[1], f[2], f[3], n[0], n[1], n[2]);

        }

    // when the alarm flag is cleared the pulldown on INT is also released
        void clearA2F() {
            uint8_t regVal;
            regVal = getSreg() & ~ds3231StatusA2F;
            setSreg(regVal);
        }
        uint8_t triggeredA2(){
            return getSreg() & ds3231StatusA2F;
        }
#endif

// helpers

#ifdef CONFIG_UNIX_TIME
    const uint8_t daysInMonth [12] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    // returns the number of seconds since 01.01.1970 00:00:00 UTC, valid for 2000..
    uint32_t systemClock::getUnixTime(struct ts t){
        uint8_t i;
        uint16_t d;
        int16_t y;
        uint32_t rv;
        if (t.year >= 2000) {
            y = t.year - 2000;
        } else {
            return 0;
        }
        d = t.mday - 1;
        for (i=1; i<t.mon; i++) {
            d += xpgm_read_byte(daysInMonth + i - 1);
        }
        if (t.mon > 2 && y % 4 == 0) {
            d++;
        }
        // count leap days
        d += (365 * y + (y + 3) / 4);
        rv = ((d * 24UL + t.hour) * 60 + t.min) * 60 + t.sec + secondsFrom1970To2000;
        return rv;
    }
#endif

    void init(const uint8_t &ctrlReg) {
        setCreg(ctrlReg);
        set32KHzOutput(0u);
    }

#ifdef USE_HELPER
    void setCurrentTime(struct ts t) {
        uint8_t i, century;
        if (t.year >= 2000) {
            century = 0x80;
            t.yearS = t.year - 2000;
        } else {
            century = 0;
            t.yearS = t.year - 1900;
        }
        uint8_t timeDate[7] = {t.sec, t.min, t.hour, t.wday, t.mday, t.mon, t.yearS};
        Wire.beginTransmission(ds3231I2CAddr);
        Wire.write(ds3231TimeCalAddr);
        for (i = 0; i <= 6; i++) {
            timeDate[i] = dec2Bcd(timeDate[i]);
            if (i == 5)
                timeDate[5] += century;
            Wire.write(timeDate[i]);
        }
        Wire.endTransmission();
    }

    void getCurrentTime(struct ts *t) {
        uint8_t timeDate[7];        //second,minute,hour,dow,day,month,year
        uint8_t century = 0;
        uint8_t i;
        uint16_t yearFull;
        Wire.beginTransmission(ds3231I2CAddr);
        Wire.write(ds3231TimeCalAddr);
        Wire.endTransmission();
        uint8_t gotData = false;
        uint32_t start = millis(); // start timeout
        while (millis() - start < ds3231TransactionTimeout) {
            if (Wire.requestFrom(ds3231I2CAddr, 7u) == 7) {
                gotData = true;
                break;
            }
            delay(2);
        }
        if (!gotData)
            return; // error timeout
        for (i = 0; i <= 6; i++) {
            uint8_t n = Wire.read();
            if (i == 5) {
                timeDate[5] = bcd2Dec(n & 0x1Fu);
                century = (n & 0x80u) >> 7u;
            } else
                timeDate[i] = bcd2Dec(n);
        }
        if (century == 1) {
            yearFull = 2000 + timeDate[6];
        } else {
            yearFull = 1900 + timeDate[6];
        }
        t->sec = timeDate[0];
        t->min = timeDate[1];
        t->hour = timeDate[2];
        t->mday = timeDate[4];
        t->mon = timeDate[5];
        t->year = yearFull;
        t->wday = timeDate[3];
        t->yearS = timeDate[6];
#ifdef CONFIG_UNIX_TIME
        t->unixtime = getUnixTime(*t);
#endif
    }
#endif

#endif

    void systemClock::setup() {
        //baseManager::setup();

#ifndef NO_CLOCK
        Wire.begin();
        init(ds3231ControlIntcn);
#endif
    }

    void systemClock::setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
        internalTime.hour = hours;
        internalTime.minute = minutes;
        internalTime.second = seconds;
        updateDevice();
    }

    void systemClock::setDate(int16_t year, uint8_t month, uint8_t day) {
        internalTime.year = year;
        internalTime.month = month;
        internalTime.day = day;
        updateDevice();
    }

    void systemClock::setDateTime(int16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes,
                                  uint8_t seconds) {
        internalTime.year = year;
        internalTime.month = month;
        internalTime.day = day;
        internalTime.hour = hours;
        internalTime.minute = minutes;
        internalTime.second = seconds;
        updateDevice();
    }

    String twoDigitInt(const uint8_t &i) {
        if (i < 10) {
            return "0" + String(i);
        } else {
            return String(i);
        }
    }

    String systemClock::getTimeStr(const String &format) const {
        String result(format);
        result.replace("%Y", String(getYear()));
        result.replace("%M", twoDigitInt(getMonth()));
        result.replace("%D", twoDigitInt(getDay()));
        result.replace("%h", twoDigitInt(getHours()));
        result.replace("%m", twoDigitInt(getMinutes()));
        result.replace("%s", twoDigitInt(getSeconds()));
        return result;
    }

    void systemClock::updateDevice() {

#ifndef NO_CLOCK
        // update the device time with internal one
        uint8_t century, yearS;
        if (internalTime.year >= 2000) {
            century = 0x80;
            yearS = internalTime.year - 2000;
        } else {
            century = 0;
            yearS = internalTime.year - 1900;
        }
        Wire.beginTransmission(ds3231I2CAddr);
        Wire.write(ds3231TimeCalAddr);
        Wire.write(dec2Bcd(internalTime.second));
        Wire.write(dec2Bcd(internalTime.minute));
        Wire.write(dec2Bcd(internalTime.hour));
        Wire.write(dec2Bcd(0)); // week day
        Wire.write(dec2Bcd(internalTime.day));
        Wire.write(dec2Bcd(internalTime.month) + century);
        Wire.write(dec2Bcd(yearS));
        Wire.endTransmission();
#endif
    }

    void systemClock::updateFromDevice() {

#ifndef NO_CLOCK
        Wire.beginTransmission(ds3231I2CAddr);
        Wire.write(ds3231TimeCalAddr);
        Wire.endTransmission();
        uint8_t gotData = false;
        uint32_t start = millis(); // start timeout
        while (millis() - start < ds3231TransactionTimeout) {
            if (Wire.requestFrom(ds3231I2CAddr, 7u) == 7) {
                gotData = true;
                break;
            }
            delay(2);
        }
        if (!gotData)
            return; // error timeout
        internalTime.second = bcd2Dec(Wire.read());
        internalTime.minute = bcd2Dec(Wire.read());
        internalTime.hour = bcd2Dec(Wire.read());
        Wire.read(); // week day
        internalTime.day = bcd2Dec(Wire.read());
        uint8_t n = Wire.read();
        internalTime.month = bcd2Dec(n & 0x1Fu);
        uint8_t century = (n & 0x80u) >> 7u;
        internalTime.year = 1900u + bcd2Dec(Wire.read()) + 100u * (century == 1u);
#endif
    }
}

