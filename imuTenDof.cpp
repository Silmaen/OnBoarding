/**
 * \author argawaen
 * \date 27/04/2020
 *
 */
#include "imuTenDof.h"
#include "I2CHelperFct.h"

namespace ob::imu {

// ---------------------------------------------------------------------------------------------------------------------
//  GYROSCOPE DATA & HELPER
// ---------------------------------------------------------------------------------------------------------------------
    PROGMEM constexpr uint8_t L3GD20_ADDRESS = 0x6Bu;
    PROGMEM constexpr uint8_t L3GD20_ID = 0xD4u;
    PROGMEM constexpr uint8_t L3GD20H_ID = 0xD7u;
    PROGMEM constexpr uint8_t GYRO_REGISTER_WHO_AM_I = 0x0Fu;
    PROGMEM constexpr uint8_t GYRO_REGISTER_CTRL_REG1 = 0x20u;
    PROGMEM constexpr uint8_t GYRO_REGISTER_CTRL_REG4 = 0x23u;
    PROGMEM constexpr uint8_t GYRO_REGISTER_CTRL_REG5 = 0x24u;
    PROGMEM constexpr uint8_t GYRO_REGISTER_OUT_X_L = 0x28u | 0x80u;
    PROGMEM constexpr float GYRO_SENSITIVITY_250DPS = 0.00875F;    // Roughly 22/256 for fixed point match
    PROGMEM constexpr float GYRO_SENSITIVITY_500DPS = 0.0175F;     // Roughly 45/256
    PROGMEM constexpr float GYRO_SENSITIVITY_2000DPS= 0.070F;      // Roughly 18/256
    PROGMEM constexpr float SENSORS_DPS_TO_RADS =  0.017453293F; /**< Degrees/s to rad/s multiplier  */
// ---------------------------------------------------------------------------------------------------------------------
//  ACCELEROMETER
// ---------------------------------------------------------------------------------------------------------------------
    PROGMEM constexpr uint8_t LSM303_ADDRESS_ACCEL              = 0x32u >> 1u;
    PROGMEM constexpr uint8_t LSM303_REGISTER_ACCEL_CTRL_REG1_A = 0x20u;
    PROGMEM constexpr uint8_t LSM303_REGISTER_ACCEL_OUT_X_L_A   = 0x28u | 0x80u;
    PROGMEM constexpr float SENSORS_GRAVITY_STANDARD            = 0.00980665F;
// ---------------------------------------------------------------------------------------------------------------------
//  MAGNETOMETER / COMPASS
// ---------------------------------------------------------------------------------------------------------------------
    PROGMEM constexpr uint8_t LSM303_ADDRESS_MAG             = 0x3Cu >> 1u;
    PROGMEM constexpr uint8_t LSM303_REGISTER_MAG_CRA_REG_M  = 0x00u;
    PROGMEM constexpr uint8_t LSM303_REGISTER_MAG_CRB_REG_M  = 0x01u;
    PROGMEM constexpr uint8_t LSM303_REGISTER_MAG_MR_REG_M   = 0x02u;
    PROGMEM constexpr uint8_t LSM303_REGISTER_MAG_OUT_X_H_M  = 0x03u;
    PROGMEM constexpr uint8_t LSM303_MAGGAIN_1_3             = 0x20u;
    PROGMEM constexpr float SENSORS_GAUSS_TO_MICROTESLA      = 100.0F;
// ---------------------------------------------------------------------------------------------------------------------
//  PRESSURE / BAROMETER
// ---------------------------------------------------------------------------------------------------------------------
    PROGMEM constexpr uint8_t BMP085_ADDRESS = (0x77u);
    PROGMEM constexpr uint8_t BMP085_REGISTER_CAL_AC1 = 0xAAu;  // R   Calibration data (16 bits)
    PROGMEM constexpr uint8_t BMP085_REGISTER_CAL_AC2 = 0xACu;  // R   Calibration data (16 bits)
    PROGMEM constexpr uint8_t BMP085_REGISTER_CAL_AC3 = 0xAEu;  // R   Calibration data (16 bits)
    PROGMEM constexpr uint8_t BMP085_REGISTER_CAL_AC4 = 0xB0u;  // R   Calibration data (16 bits)
    PROGMEM constexpr uint8_t BMP085_REGISTER_CAL_AC5 = 0xB2u;  // R   Calibration data (16 bits)
    PROGMEM constexpr uint8_t BMP085_REGISTER_CAL_AC6 = 0xB4u;  // R   Calibration data (16 bits)
    PROGMEM constexpr uint8_t BMP085_REGISTER_CAL_B1 = 0xB6u;  // R   Calibration data (16 bits)
    PROGMEM constexpr uint8_t BMP085_REGISTER_CAL_B2 = 0xB8u;  // R   Calibration data (16 bits)
    PROGMEM constexpr uint8_t BMP085_REGISTER_CAL_MB = 0xBAu;  // R   Calibration data (16 bits)
    PROGMEM constexpr uint8_t BMP085_REGISTER_CAL_MC = 0xBCu;  // R   Calibration data (16 bits)
    PROGMEM constexpr uint8_t BMP085_REGISTER_CAL_MD = 0xBEu;  // R   Calibration data (16 bits)
    PROGMEM constexpr uint8_t BMP085_REGISTER_CHIPID = 0xD0u;
    PROGMEM constexpr uint8_t BMP085_REGISTER_VERSION = 0xD1u;
    PROGMEM constexpr uint8_t BMP085_REGISTER_SOFTRESET = 0xE0u;
    PROGMEM constexpr uint8_t BMP085_REGISTER_CONTROL = 0xF4u;
    PROGMEM constexpr uint8_t BMP085_REGISTER_TEMPDATA = 0xF6u;
    PROGMEM constexpr uint8_t BMP085_REGISTER_PRESSUREDATA = 0xF6u;
    PROGMEM constexpr uint8_t BMP085_REGISTER_READTEMPCMD = 0x2Eu;
    PROGMEM constexpr uint8_t BMP085_REGISTER_READPRESSURECMD = 0x34u;
    struct {
        int16_t ac1;
        int16_t ac2;
        int16_t ac3;
        uint16_t ac4;
        uint16_t ac5;
        uint16_t ac6;
        int16_t b1;
        int16_t b2;
        int16_t mc;
        int16_t md;
    } bmp085Coeffs;
    constexpr byte bmpMode = 3;

// ---------------------------------------------------------------------------------------------------------------------
// --- GLOBAL SECTION ---
// ---------------------------------------------------------------------------------------------------------------------

    static void L3GD_startup(){
        /* Make sure we have the correct chip ID since this checks
         for correct address and that the IC is properly connected */
        byte gyroId = read8(L3GD20_ADDRESS, GYRO_REGISTER_WHO_AM_I);
        if ((gyroId == L3GD20_ID) || (gyroId == L3GD20H_ID)) {
            /* STARTUP SEQUENCE
             * 1. Write CTRL_REG2          0      0   HPM1   HPM0  HPCF3  HPCF2  HPCF1  HPCF0
             * 2. Write CTRL_REG3     I1Int1 I1boot  HLact  PP_OD I2DRDY  I2WTM I2ORun I2Empt
             * 3. Write CTRL_REG4        BDU    BLE    FS1    FS0      0      0      0    SIM
             * 5. Write REFERENCE       REF7   REF6   REF5   REF4   REF3   REF2   REF1   REF0
             * 6. Write INT1_THS    *********************************************************
             * 7. Write INT1_DUR    *********************************************************
             * 8. Write INT1_CFG    *********************************************************
             * 9. Write CTRL_REG5       BOOT FIFOEN      0   HPen I1Sel1 I1sel2 O_Sel1 o_Sel0
             * 10. Write CTRL_REG1       DR1    DR0    BW1    BW0     PD    Zen    Yen    Xen
             *                             1      1      1      1      1      1      1      1    0xFF
             */
            write8(L3GD20_ADDRESS, GYRO_REGISTER_CTRL_REG1, 0x00u); // PowerDown
            write8(L3GD20_ADDRESS,GYRO_REGISTER_CTRL_REG4, 0x00); // +/- 250 DPS max precision, lowest range
            //write8(L3GD20_ADDRESS, GYRO_REGISTER_CTRL_REG5, 0x80); // BOOT, FIFO disable, Interupts disable
            write8(L3GD20_ADDRESS, GYRO_REGISTER_CTRL_REG1, 0xFFu); // [ODR 760Hz, cutoff 100Hz] [Normal, all axis enabled]
        }
    }

    void setup() {
#ifndef NO_AHRS
        /* Enable I2C */
        wireInit();

        //  GYROSCOPE INIT
        L3GD_startup();
        //  END GYROSCOPE INIT

        //  ACCELEROMETER INIT
        /*// Enable the accelerometer (100Hz)
        write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG1_A, 0x57u);*/
        // Enable the accelerometer (1344Hz)
        write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG1_A, 0x97u);
        //  MAGNETOMETER INIT
        // Enable continuous conversion
        write8(LSM303_ADDRESS_MAG, LSM303_REGISTER_MAG_MR_REG_M, 0x00u);
        // Enable the magnetometer (200Hz & Temperature disabled)
        write8(LSM303_ADDRESS_MAG, LSM303_REGISTER_MAG_CRA_REG_M, 0x1Cu);
        // Set the gain to a known level
        write8(LSM303_ADDRESS_MAG, LSM303_REGISTER_MAG_CRB_REG_M, LSM303_MAGGAIN_1_3);
        //  END MAGNETOMETER INIT

        // BAROMETER
        /* Coefficients need to be read once */
        bmp085Coeffs.ac1 = read16Msb(BMP085_ADDRESS, BMP085_REGISTER_CAL_AC1);
        bmp085Coeffs.ac2 = read16Msb(BMP085_ADDRESS, BMP085_REGISTER_CAL_AC2);
        bmp085Coeffs.ac3 = read16Msb(BMP085_ADDRESS, BMP085_REGISTER_CAL_AC3);
        bmp085Coeffs.ac4 = read16Msb(BMP085_ADDRESS, BMP085_REGISTER_CAL_AC4);
        bmp085Coeffs.ac5 = read16Msb(BMP085_ADDRESS, BMP085_REGISTER_CAL_AC5);
        bmp085Coeffs.ac6 = read16Msb(BMP085_ADDRESS, BMP085_REGISTER_CAL_AC6);
        bmp085Coeffs.b1 = read16Msb(BMP085_ADDRESS, BMP085_REGISTER_CAL_B1);
        bmp085Coeffs.b2 = read16Msb(BMP085_ADDRESS, BMP085_REGISTER_CAL_B2);
        bmp085Coeffs.mc = read16Msb(BMP085_ADDRESS, BMP085_REGISTER_CAL_MC);
        bmp085Coeffs.md = read16Msb(BMP085_ADDRESS, BMP085_REGISTER_CAL_MD);
#endif
    }

    void frame() {
#ifndef NO_AHRS

#endif
    }
// ---------------------------------------------------------------------------------------------------------------------
// --- LSM303 SECTION ---
// ---------------------------------------------------------------------------------------------------------------------

    vec3D getAcceleration() {
#ifdef NO_AHRS
        return vec3D{0.0, 0.0, 0.0};
#else
        // read the accelerometer
        return vec3D{ (float)((int32_t)read16Lsb(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_OUT_X_L_A) >> 4u) * SENSORS_GRAVITY_STANDARD,
                      (float)((int32_t)read16Lsb(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_OUT_X_L_A + 2) >> 4u) * SENSORS_GRAVITY_STANDARD,
                      (float)((int32_t)read16Lsb(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_OUT_X_L_A + 4) >> 4u) * SENSORS_GRAVITY_STANDARD};
#endif
    }

    vec3D getMagnetic() {
#ifdef NO_AHRS
        return vec3D{0.0, 0.0, 0.0};
#else
        // Read the magnetometer
        return vec3D{ (float)read16Msb(LSM303_ADDRESS_MAG, LSM303_REGISTER_MAG_OUT_X_H_M) / 11.0f,
                      (float)read16Msb(LSM303_ADDRESS_MAG, LSM303_REGISTER_MAG_OUT_X_H_M + 2)/ 11.0f,
                      (float)read16Msb(LSM303_ADDRESS_MAG, LSM303_REGISTER_MAG_OUT_X_H_M + 4) / 9.8f};

#endif
    }

// ---------------------------------------------------------------------------------------------------------------------
// --- L3GD20 SECTION ---
// ---------------------------------------------------------------------------------------------------------------------

    vec3D getRotationRate() {
#ifdef NO_AHRS
        return vec3D{0.0, 0.0, 0.0};
#else
        // Read the gyroscope
       return vec3D{ (float)read16Lsb(L3GD20_ADDRESS, GYRO_REGISTER_OUT_X_L ) * GYRO_SENSITIVITY_250DPS,
                      (float)read16Lsb(L3GD20_ADDRESS, GYRO_REGISTER_OUT_X_L + 2) * GYRO_SENSITIVITY_250DPS,
                      (float)read16Lsb(L3GD20_ADDRESS, GYRO_REGISTER_OUT_X_L + 4) * GYRO_SENSITIVITY_250DPS};

#endif
    }

// ---------------------------------------------------------------------------------------------------------------------
// --- BMP085 SECTION ---
// ---------------------------------------------------------------------------------------------------------------------

    int32_t getRawTemperature() {
        write8(BMP085_ADDRESS, BMP085_REGISTER_CONTROL, BMP085_REGISTER_READTEMPCMD);
        delay(5);
        int32_t x1 = ((read16Msb(BMP085_ADDRESS, BMP085_REGISTER_TEMPDATA) - (uint32_t) bmp085Coeffs.ac6) *
                      (uint32_t) bmp085Coeffs.ac5) >> 15u;
        return x1 + ((int32_t) bmp085Coeffs.mc << 11u) / (x1 + (int32_t) bmp085Coeffs.md);
    }

    uint32_t getRawPressure() {
        write8(BMP085_ADDRESS, BMP085_REGISTER_CONTROL, BMP085_REGISTER_READPRESSURECMD + (bmpMode << 6u));
        delay(26);
        return (read(BMP085_ADDRESS, BMP085_REGISTER_PRESSUREDATA, 3u) >> (8u - bmpMode));
    }

    float getTemperature() {
        return ((getRawTemperature() + 8) >> 4) / 10.0f;
    }

    float getPressure() {
#ifndef NO_AHRS
        int32_t p;
        /* Pressure compensation */
        int32_t b6 = getRawTemperature() - 4000; // 4000 = 25C
        uint32_t b4 = ((uint32_t) bmp085Coeffs.ac4 * (uint32_t) (
                (((((int32_t) bmp085Coeffs.ac3 * b6) >> 13u) + (((int32_t) bmp085Coeffs.b1 * (b6 * b6 >> 12u)) >> 16u) +
                  2) >> 2) + 32768)) >> 15u;
        int32_t b7 = (getRawPressure() - (((((int32_t) bmp085Coeffs.ac1 * 4 +
                                             (((int32_t) bmp085Coeffs.b2 * (b6 * b6 >> 12u) +
                                               (int32_t) bmp085Coeffs.ac2 * b6) >> 11u)) << bmpMode) + 2) / 4)) *
                     (50000u >> bmpMode);
        if (b7 < 0x80000000) {
            p = (b7 << 1u) / b4;
        } else {
            p = (b7 / b4) << 1u;
        }
        return (p + (((((p >> 8u) * (p >> 8u) * 3038u) >> 16u) + ((-7357 * p) >> 16u) + 3791u) >> 4u));
#else
        return 0;
#endif
    }

    static float qnh = 101710.0;

    void setupQnh(float qnhValue) {
        qnh = qnhValue * 100.f;
    }

    void setupCurrentAltitude(float currentTheoreticalAltitude) {
        qnh = getPressure() / pow(1.0f - (currentTheoreticalAltitude / 44330.0f), 5.255f);
    }

    float getAltitude() {
        return 44330.0f * (1.0f - pow(getPressure() / qnh, 0.190295f));
    }

// ---------------------------------------------------------------------------------------------------------------------
// --- END SECTION ---
// ---------------------------------------------------------------------------------------------------------------------

}
