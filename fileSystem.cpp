/**
 * \author argawaen
 * \date 09/04/2020
 */
#include "fileSystem.h"
//#include <SdFat/src/SdFat.h>

template<> ob::core::fileSystem ob::baseManager<ob::core::fileSystem>::instance{ob::core::fileSystem()};

constexpr uint8_t sdCs = SS;

//SdFat sdCard;

namespace ob::core {

    fileSystem::fileSystem() {
        //sdCard.begin(sdCs);
    }
}
