/**
 * \author argawaen
 * \date 09/04/2020
 */
#include "fileSystem.h"
#include "allManager.h"

template<> ob::core::fileSystem ob::baseManager<ob::core::fileSystem>::instance{ob::core::fileSystem()};

constexpr uint8_t sdCs = 4;

namespace ob::core {

    // -----------------------------------------------------------------------------------------------------------------
    fileSystem::fileSystem() {
        managerName = F("file system Manager");
        hasSdCard = false;
    }

    void fileSystem::setup() {
        baseManager::setup();
#ifndef NO_SDCARD
        hasSdCard = true;
        if (!SD.begin(sdCs)) {
            hasSdCard = false;
            communicator.send(F("ERROR: no SD card."));
            status.fallInError(statusCode::SDCardError);
        }
#else
        hasSdCard = false;
#endif
    }

    bool fileSystem::exists(const path &p) const {
        if (hasSdCard) {
            return (SD.exists(p.pathName));
        }
        return false;
    }

    File fileSystem::open(const path &p, openMode io) const {
        if (hasSdCard) {
            switch (io) {
                case openMode::Read:
                    SD.open(p.pathName, O_READ);
                    break;
                case openMode::Write:
                    SD.open(p.pathName, O_WRITE);
                    break;
                case openMode::Append:
                    SD.open(p.pathName, O_APPEND);
                    break;
            }
        } else {
            return File();
        }
    }
}
