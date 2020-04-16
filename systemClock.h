/**
 * \author argawaen
 * \date 10/04/2020
 *
 */
#pragma once
#include "baseManager.h"

namespace ob::core {
    /**
     * \brief manager for the external system clock
     */
    class systemClock : public baseManager<systemClock> {
        friend class baseManager<systemClock>;
    public:
        /**
         * \brief to be called at setup time
         */
        void setup() override;
        /**
         * \brief setup the current time
         * \param hours the hour in clock (24 hours format)
         * \param minutes the minutes on the clock
         * \param seconds the seconds ont eh clock
         */
        void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
        /**
         * \brief set the current date
         * \param year the current year
         * \param month the current month
         * \param day  the current day of the month
         */
        void setDate(int16_t year, uint8_t month, uint8_t day);
        /**
         * \brief setup both date and time
         * \param year the current year
         * \param month the current month
         * \param day the current day
         * \param hours the hours on the clock (24 hours format)
         * \param minutes the minutes on the clock
         * \param seconds the seconds on the clock
         */
        void setDateTime(int16_t year, uint8_t month, uint8_t day,uint8_t hours, uint8_t minutes, uint8_t seconds);
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
        [[nodiscard]] String getTimeStr(const String &format = "%Y %M %D %h:%m:%s") const;

        /**
         * \brief get the current year
         * \return the current year
         */
        [[nodiscard]] uint16_t getYear() const { return internalTime.year; }

        /**
         * \brief get the current month
         * \return the current month
         */
        [[nodiscard]] uint8_t getMonth() const { return internalTime.month; }

        /**
         * \brief get the current day
         * \return the current day
         */
        [[nodiscard]] uint8_t getDay() const { return internalTime.day; }

        /**
         * \brief get the current hours
         * \return the current hours
         */
        [[nodiscard]] uint8_t getHours() const { return internalTime.hour; }

        /**
         * \brief get the current minutes
         * \return the current minutes
         */
        [[nodiscard]] uint8_t getMinutes() const { return internalTime.minute; }

        /**
         * \brief get the current seconds
         * \return the current seconds
         */
        [[nodiscard]] uint8_t getSeconds() const { return internalTime.second; }

#ifdef CONFIG_UNIX_TIME
        uint32_t getUnixTime(struct ts t);
#endif
    private:
        /**
        * \brief base constructor
        */
        systemClock(){
            managerName=F("System Time Manager");
        }
        /**
         * \brief destructor
         */
        ~systemClock() = default;

        /**
         * \brief private copy constructor to avoid copy
         */
        systemClock(const systemClock &) = default;

        /**
         * \brief update the device with the internal date
         */
        void updateDevice();
        /**
         * \brief update the internal date with the device
         */
        void updateFromDevice();
        /**
         * \brief very simple struct to handle minimal timestamp
         */
        struct simpleTimeStamp {
            uint16_t year = 0;
            uint8_t month = 0;
            uint8_t day = 0;
            uint8_t hour = 0;
            uint8_t minute = 0;
            uint8_t second = 0;
        } internalTime;
    };
}
