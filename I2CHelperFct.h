/**
 * \author argawaen
 * \date 30/04/2020
 *
 */
#pragma once

#include <Arduino.h>
#include <Wire.h>

#define I2CREQUESTCHECKS

namespace ob {

    /**
     * \brief Write the value of the register of given device
     * \param address address of the device
     * \param reg register address
     * \param value value to write
     */
    void write8(byte address, byte reg, byte value);

    /**
     * \brief read the value of the register on the device
     * \param address address of the device
     * \param reg register name
     * \return the read value
     */
    byte read8(byte address, byte reg);

    /**
     * \brief read the value of the register on the device (16bits version)
     * \param address address of the device
     * \param reg register name
     * \return the read value
     */
    uint16_t read16(byte address, byte reg);

    /**
     * \brief read the value of the register on the device (16bits signed version)
     * \param address address of the device
     * \param reg register name
     * \return the read value
     */
    int16_t readS16(byte address, byte reg);

    uint32_t read(byte address, byte reg, byte size);

    bool requestFrom(byte address, byte reg, byte size);
}