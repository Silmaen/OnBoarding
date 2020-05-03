/**
 * \author argawaen
 * \date 27/04/2020
 *
 */
#pragma once

#include "globalConfig.h"

namespace ob::imu {

    /**
     * \brief to be called at setup time
     */
    void setup();

    /**
     * \brief to be called Each Frame
     */
    void frame();

    /**
     * \brief get instant acceleration vector (in relative coordinates)
     * \return the acceleration
     */
    vec3D getAcceleration();

    /**
     * \brief get instant rotation rate (in relative coordinates)
     * \return the rotation rate
     */
    vec3D getRotationRate();

    /**
     * \brief get instant magnetic field (in relative coordinates)
     * \return the magnetic field
     */
    vec3D getMagnetic();

    /**
     * \brief read the barometer altitude
     * \return altitude read
     */
    float getAltitude();

    /**
     * \brief read the barometer value
     * \return the atmospheric pressure
     */
    float getPressure();

    /**
     * \brief read the current temperature
     * \return the instant temperature
     */
    float getTemperature();

    /**
     * \brief helper function to calibrate the altimeter based on QNH value
     * \param qnhValue the actual QNH value
     */
    void setupQnh(float qnhValue);

    /**
     * \brief helper function to setup altimeter based on the current theoretical altitude
     * \param currentTheoreticalAltitude the current altitude
     */
    void setupCurrentAltitude(float currentTheoreticalAltitude);

    void printinner();
}
