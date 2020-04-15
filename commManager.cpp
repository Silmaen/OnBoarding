/**
 * \author argawaen
 * \date 14/04/2020
 *
 */
#include "commManager.h"

template<> ob::comm::commManager ob::baseManager<ob::comm::commManager>::instance = ob::comm::commManager();

namespace ob::comm {

    void commManager::setup() {
        baseManager::setup();
        Serial.begin(115200);
    }

    void commManager::frame() {
        baseManager::frame()
        while (Serial.available()){

        }

    }

    void commManager::send(const String &message) {
        Serial.print(message);
    }

}
