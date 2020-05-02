/**
 * \author argawaen
 * \date 08/04/2020
 */
#include "logger.h"
#include "allManager.h"

// instantiation of the manager
template<> ob::core::logger ob::baseManager<ob::core::logger>::instance{ob::core::logger()};

namespace ob::core {
    // -----------------------------------------------------------------------------------------------------------------
    ob::core::logger::logger() : verbosity(logLevel::Warning) {
        managerName = F("Logger Manager");
        String baseFilename = clock.getTimeStr(F("LOG/log_%Y%M%D_%h%m"));
        logFilename = baseFilename + ".log";
        uint8_t t = 1;
        while (filesys.exists(path(logFilename + ".log"))) {
            logFilename = baseFilename + "_" + String(t++) + ".log";
            if (t > 10) {
                communicator.send(F("ERROR: no file space left"));
                status.fallInError(statusCode::LoggerError);
            }
        }
    }

    // -----------------------------------------------------------------------------------------------------------------
    void ob::core::logger::logMessage(const baseManager &mng, const String &msg, ob::core::logLevel msgLevel) {
        if (msgLevel > verbosity) return;
        File a = filesys.open(path(logFilename), openMode::Append);
        a.println(clock.getTimeStr(F("%Y-%M-%D %h:%m:%s [")) + mng.getName() + F("] ") + msg);
        a.close();
    }

}