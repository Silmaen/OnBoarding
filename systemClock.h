/**
 * \author argawaen
 * \date 10/04/2020
 *
 */
#pragma once
#include "globalConfig.h"

namespace ob::time {
    /**
     * \brief to be called at setup time
     */
    void setup();

    /**
     * \brief setup both date and time
     * \param year the current year
     * \param month the current month
     * \param day the current day
     * \param hours the hours on the clock (24 hours format)
     * \param minutes the minutes on the clock
     * \param seconds the seconds on the clock
     */
    void setDateTime(const uint16_t &year, const uint8_t &month, const uint8_t &day, const uint8_t &hour,
                     const uint8_t &minute, const uint8_t &second);

    /**
     * \brief get a string of the date in the given format
     * \param format the format for output
     *
     * for is a string where keywords will be replaced:
     *  * %Y  the year
     *  * %M  the month
     *  * %D  the month
     *  * %h  the hour
     *  * %m  the minute
     *  * %s  the second
     * \return the date in the given format
     */
    void getTimeStamp(char *res);

    /**
     * \brief get the temperature of the clock device
     * \return the temperature of the clock device
     */
    [[nodiscard]] float getClockTemperature();
}

