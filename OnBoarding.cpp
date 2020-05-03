/**
 * \author argawaen
 * \date 08/04/2020
 */
#include "systemClock.h"
#include "systemComm.h"
#include "imuTenDof.h"

using namespace ob;

void setup() {
    time::setup();
    comm::setup();
    imu::setup();
}

void loop() {
    imu::frame();
    comm::frame();
    comm::send(String(time::getClockTemperature()) + F(" C")
               + imu::getAcceleration().toStr()
               + imu::getRotationRate().toStr()
               + imu::getMagnetic().toStr()
               /*+ F(" ") + String(imu::getPressure())*/
               + F(" ") + String(imu::getAltitude())
               + F(" ") + String(imu::getTemperature())
    );
    delay(2000);
}
