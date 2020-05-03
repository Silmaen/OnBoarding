/**
 * \author argawaen
 * \date 30/04/2020
 *
 */
#pragma once
#include "globalConfig.h"

namespace ob {

    PROGMEM constexpr uint8_t wireTransactionTimeout = 100;

    /**
     * \brief initilize I2C
     */
    void wireInit();

    /**
     * \brief Write the value of the register of given device
     * \param address address of the device
     * \param reg register address
     * \param value value to write
     */
    void write8(u8 address, u8 reg, u8 value);

    /**
     * \brief read the value of the register on the device
     * \param address address of the device
     * \param reg register address
     * \return the read value
     */
    [[nodiscard]] s8 read8(u8 address, u8 reg);

    /**
     * \brief read the value of the register on the device (16bits version MSB)
     * \param address address of the device
     * \param reg register address
     * \return the read value
     */
    [[nodiscard]] s16 read16Msb(u8 address, u8 reg);
    /**
     * \brief read the value of the register on the device (16bits version LSB)
     * \param address address of the device
     * \param reg register address
     * \return the read value
     */
    [[nodiscard]] s16 read16Lsb(u8 address, u8 reg);

    /**
     * \brief read the value of the register on the device
     * \param address address of the device
     * \param reg register begining address
     * \param size the size in bytes to read (max 4, warning: no checks)
     * \param msb true (default): read Most Signifiant Byte fiste, else read Least Significant Byte first
     * \return the read value
     */
    [[nodiscard]] s32 read(u8 address, u8 reg, u8 size, bool msb=true);

    /**
     * \brief Request the send of the register content
     * \param address device address
     * \param reg register address
     * \param size the requested size in bytes
     * \return true when data are ready to be read
     */
    [[nodiscard]] bool requestFrom(u8 address, u8 reg, u8 size);
}