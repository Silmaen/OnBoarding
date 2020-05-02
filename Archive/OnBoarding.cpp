/**
 * \author argawaen
 * \date 08/04/2020
 */
#include "allManager.h"

using namespace ob;

void setupManagers() {
    status.setup();       // initialize first!
    clock.setup();
    filesys.setup();
    communicator.setup();
    logg.setup(); // must be initialized AFTER filesys and clock
    cmd.setup();
}

void frameManagers() {
    status.frame();
    clock.frame();
    filesys.frame();
    communicator.frame();
    logg.frame();
    cmd.frame();
}

void setup() {
    setupManagers();
}

void loop() {
    frameManagers();
#ifdef DEBUG
    String a = clock.getTimeStr(F("%Y-%M-%D %h:%m:%s ["));
    a += status.getName() + F("] status: ") += status.getStatusName() + F("\n");
    communicator.send(a);
    delay(500);
#endif
}
