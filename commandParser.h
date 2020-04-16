/**
 * \author argawaen
 * \date 16/04/2020
 *
 */
#pragma once

#include "baseManager.h"

namespace ob::core {

    class commandParser : public baseManager<commandParser> {
        friend class baseManager<commandParser>;

    public:
        /**
         * \brief parse and treat the command
         * \param cmd the command to parse
         */
        void treatCommand(const char *cmd) {
            treatCommand(String(cmd));
        }

        /**
         * \brief parse and treat the command
         * \param cmd the command to parse
         */
        void treatCommand(const String &cmd);

    private:
        /**
         * \brief base constructor
         */
        commandParser() {
            managerName = F("Command parser");
        }
        /**
         * \brief destructor
         */
        ~commandParser() = default;

        /**
         * \brief private copy constructor to avoid copy
         */
        commandParser(const commandParser &) = default;
        /**
         * \brief get a string for the up time
         * \return the uptime in Days, hours, minutes, seconds (as float)
         */
        String uptime();

        /**
         * \brief set the clock time
         * \param parameter the input time
         * \return true if everything goes well
         */
        bool setTime(const String &parameter);
    };

}

