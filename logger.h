/**
 * \author argawaen
 * \date 08/04/2020
 *
 */
#pragma once
#ifdef RECEIVER
#error "no logger on the receiver"
#endif
#include "baseManager.h"

namespace ob::core{

    /**
     * \brief the different log level
     */
    enum class logLevel {
        Silent,     ///< nothing log
        FatalErrors,///< log fatal error
        Errors,     ///< log error
        Warning,    ///< log Warnings (default)
        Extra,      ///< log Extra
        Everything, ///< log all messages
    };

    /**
     * \brief class to handle log on SD Card
     */
    class logger: public baseManager<logger> {
        friend class baseManager<logger>;
    public:
        /**
         * \brief define the level of verbosity
         * \param lvl the new level of verbosity
         */
        void setVerbosity(logLevel lvl){ verbosity = lvl;}
        /**
         * \brief getTime the actual level of verbosity
         * \return the actual level of verbosity
         */
        logLevel getVerbosity(){return verbosity;}
        /**
         * \brief log a message into the new logger
         * \param msg the message to log
         * \param msgLevel the verbosity level of the message
         */
        void logMessage(const String& msg, logLevel msgLevel);
        /**
         * \brief to be called every frame
         */
        void frame() override;
    private:
        /**
        * \brief base constructor
        */
        logger();
        /**
         * \brief destructor
         */
        ~logger() = default;
        /**
         * \brief private copy constructor to avoid copy
         */
        logger(const logger &) = default;
        logLevel verbosity; ///< actual verbosity
    };

}
