#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

/**
 * \brief the radio module
 */
RF24 radio(10,9);

const byte device[] = "OnBoarding";
const byte bridge[] = "OnBoardingBridge";
constexpr uint16_t maxBufferSize = 250;

void setup() {
    Serial.begin(115200);
    while(!Serial) {} // wait for serial connection
    Serial.println("Begin");
    if (radio.begin()) {
        //
        radio.setPALevel(RF24_PA_LOW);
        radio.openReadingPipe(1, bridge);
        radio.openWritingPipe(device);
        //radio.flush_rx();
        radio.startListening();
        radio.printDetails();
        //
    }else{
        Serial.println("Error while initializing radio");
        while(1){}
    }
    Serial.println("Press enter to begin...");
    while(!Serial.available()) {}
    Serial.flush();
    Serial.println("Starting");
}

char buffer[maxBufferSize];

void loop() {
    // transmit from serial to radio
    //if (Serial.available()){
        uint16_t index = 0;
        while (Serial.available() && index < maxBufferSize)
            buffer[index++] = Serial.read();
        //Serial.println("buffer size to send: " + String(index));
        buffer[index] = '\0';
        if (index > 0) {
            radio.stopListening();
            radio.write(buffer, index);
            radio.startListening();
            Serial.print(">>> ");
            Serial.println(buffer);
        }
    //}

    // transmit from radio, to serial
    /*if (radio.available()){
        uint16_t index = 0;
        while (radio.available() && index < maxBufferSize)
            radio.read(&buffer[index++],1);
        radio.flush_rx();
        Serial.println("buffer size received: " + String(index));
        buffer[index] = '\0';
        if (index > 0) {
            Serial.print("<<< '");
            Serial.print(buffer);
            Serial.println("'");
        }
    }*/
}
