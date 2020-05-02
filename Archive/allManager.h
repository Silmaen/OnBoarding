/**
 * \author argawaen
 * \date 17/04/2020
 *
 */

#pragma once

#include "statusManager.h"
#include "commandParser.h"
#include "commManager.h"
#include "systemClock.h"
#include "fileSystem.h"
#include "logger.h"

namespace ob {

    static core::statusManager &status = core::statusManager::get();
    static comm::commManager &communicator = comm::commManager::get();
    static core::systemClock &clock = core::systemClock::get();
    static core::fileSystem &filesys = core::fileSystem::get();
    static core::logger &logg = core::logger::get();
    static core::commandParser &cmd = core::commandParser::get();

}
