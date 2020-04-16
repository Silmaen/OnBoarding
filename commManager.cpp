/**
 * \author argawaen
 * \date 14/04/2020
 *
 */
#include "commManager.h"

#ifndef NO_RF

#include <RF24.h>

#endif

#include "commandParser.h"

template<> ob::comm::commManager ob::baseManager<ob::comm::commManager>::instance = ob::comm::commManager();

#ifndef NO_RF
RF24 radio(10, 9);
#endif

namespace ob::comm {
    core::commandParser &command = core::commandParser::get();

    void commManager::setup() {
        baseManager::setup();
        // initialize the serial communication
        Serial.begin(115200);

#ifndef NO_RF
        // initialize the RF communication
        radio.begin();
        // set communication channel [0 - 125]
        radio.setChannel(4);
        // set Power Amplifier value
        // RF24_PA_MIN    RF24_PA_LOW    RF24_PA_HIGH    RF24_PA_MAX
        //   -18dBm         -12dBm          -6dBM            0dBm
        radio.setPALevel(RF24_PA_MAX);
        // Set the transmission data rate (Higher rate reduce range)
        // RF24_250KBPS    RF24_1MBPS    RF24_2MBPS
        radio.setDataRate(RF24_1MBPS);
        // ensure the auto acknowledgment is activated
        radio.setAutoAck(true);
        // define delay & number of retries
        radio.setRetries(2, 15);
        // set the CRC length (8-bits for performances)
        // possible: RF24_CRC_DISABLED         RF24_CRC_8          RF24_CRC_16
        //         only if autoAck disabled       8bits               16bits
        radio.setCRCLength(RF24_CRC_8);

        // defining pipes
        radio.openWritingPipe((uint8_t *) "Receiver4586");             // My name (for output)
        radio.openReadingPipe(1, (uint8_t *) "OnBoarding4586"); // name of the other device (listen to)

        // starting all
        radio.startListening();
        radio.powerUp();
#endif
    }

    void commManager::frame() {
        baseManager::frame();
        if (Serial.available()) {
            uint16_t index = 0;
            while (Serial.available() && index < maxBufferSize)
                buffer[index++] = Serial.read();
            buffer[index] = '\0';
            if (index > 0) {
                command.treatCommand(buffer);
            }
        }
#ifndef NO_RF
        // transmit from radio, to serial
        if (radio.available()) {
            uint16_t index = 0;
            while (radio.available() && index < maxBufferSize)
                radio.read(&buffer[index++], 1);
            radio.flush_rx(); // if message is too long.... abort receiving!!
            buffer[index] = '\0';
            if (index > 0) {
                if (buffer[0] == '$') {
                    command.treatCommand(buffer);
                }
            }
        }
#endif
    }

    void commManager::send(const String &message) const {
        // send through serial
        Serial.print(message);
#ifndef NO_RF
        // send through RF
        radio.stopListening();
        radio.write(message.c_str(), message.length());
        radio.startListening();
#endif
    }

}
