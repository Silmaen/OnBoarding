/**
 * \author argawaen
 * \date 08/04/2020
 */
#ifndef RECEIVER
#include "logger.h"
#include "statusManager.h"

ob::core::logger::logger(): verbosity(logLevel::Warning) {
    managerName="LoggerManager";
}

void ob::core::logger::logMessage(const String&  msg, ob::core::logLevel msgLevel) {

}

void ob::core::logger::frame() {
    baseManager::frame();
    //
}

#endif
