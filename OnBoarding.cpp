/**
 * \author argawaen
 * \date 08/04/2020
 */
#include <Arduino.h>
#include "statusManager.h"
#include "commManager.h"
#ifndef RECEIVER
#include "systemClock.h"
#endif

using namespace ob;

#define DEBUG
core::statusManager& status = core::statusManager::get();
comm::commManager& communicator = comm::commManager::get();
#ifndef RECEIVER
core::systemClock& clock = core::systemClock::get();
#endif

void setup() {
    status.setup();
#ifndef RECEIVER
    clock.setup();
#endif
    communicator.setup();
}

void loop() {
    status.frame();
#ifndef RECEIVER
    clock.frame();
#endif
    communicator.frame();

#ifdef DEBUG
#ifndef RECEIVER
    String a = clock.getTimeStr(F("%Y-%M-%D %h:%m:%s ["));
#else
    String a = F("[");
#endif
    a += status.getName() + F("] status: ") += status.getStatusName() + F("\n");
    communicator.send(a);
    delay(500);
#endif
}
