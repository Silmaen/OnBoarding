/**
 * \author argawaen
 * \date 27/04/2020
 *
 */
#pragma once

#include "systemComm.h"
#include "systemClock.h"

namespace ob::comm {

    void setup() {
        Serial.begin(115200);
    }

    void frame() {

    }

    static char timestamp[22];

    void send(const String &msg) {
        time::getTimeStamp(timestamp);
        Serial.print(timestamp);
        Serial.print(' ');
        if (msg.endsWith("\n"))
            Serial.print(msg);
        else
            Serial.println(msg);
    }
}
