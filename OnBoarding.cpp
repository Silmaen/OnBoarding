/**
 * \author argawaen
 * \date 08/04/2020
 */


#include <Arduino.h>
#include "statusManager.h"
#include "commManager.h"
#include "systemClock.h"

using namespace ob;

//#define DEBUG
core::statusManager& status = core::statusManager::get();
comm::commManager& communicator = comm::commManager::get();
core::systemClock& clock = core::systemClock::get();

void setup() {
    status.setup();
    clock.setup();
    communicator.setup();
}

void loop() {
    status.frame();
    clock.frame();
    communicator.frame();

#ifdef DEBUG
    String a = clock.getTimeStr(F("%Y-%M-%D %h:%m:%s ["));
    a += status.getName() + F("] status: ") += status.getStatusName() + F("\n");
    communicator.send(a);
    delay(500);
#endif
}
