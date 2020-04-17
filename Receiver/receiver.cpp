/**
 * \author argawaen
 * \date 16/04/2020
 *
 */

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

/**
 * \brief the radio module
 */

const byte device[] = "OnBoarding";
const byte bridge[] = "OnBoardingBridge";
constexpr uint16_t maxBufferSize = 250;

constexpr uint32_t total = 1000;
RF24 radio(10, 9);

void failingLoop() {
    constexpr uint32_t wait = total / 8;
    while (1) {
        delay(wait);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(wait);
        digitalWrite(LED_BUILTIN, LOW);
    }
}

void waitingUsbLoop() {
    constexpr uint32_t wait1 = total / 4;
    constexpr uint32_t wait2 = wait1 / 2;
    while (!Serial) {
        delay(wait1 + wait2);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(wait1);
        digitalWrite(LED_BUILTIN, LOW);
        delay(wait2);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(wait1);
        digitalWrite(LED_BUILTIN, LOW);
    }
}

void waitingUsbInputLoop() {
    constexpr uint32_t wait1 = total / 4;
    constexpr uint32_t wait2 = wait1 / 2;
    while (!Serial.available()) {
        delay(wait1 + wait2);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(wait1);
        digitalWrite(LED_BUILTIN, LOW);
        delay(wait2);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(wait1);
        digitalWrite(LED_BUILTIN, LOW);
    }
    Serial.read();
}

bool initRadio() {
    // initialize the device
    if (!radio.begin()) {
        Serial.println("Error while initializing radio");
        failingLoop();
    }
    // set communication channel [0 - 125]
    radio.setChannel(4);
    // set Power Amplifier value
    // RF24_PA_MIN    RF24_PA_LOW    RF24_PA_HIGH    RF24_PA_MAX
    //   -18dBm         -12dBm          -6dBM            0dBm
    radio.setPALevel(RF24_PA_LOW);
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

    radio.enableDynamicPayloads();


    // defining pipes
    radio.openWritingPipe((uint8_t *) "Receiver4586");             // My name (for output)
    radio.openReadingPipe(1, (uint8_t *) "OnBoarding4586"); // name of the other device (listen to)

    // starting all
    radio.startListening();
    radio.powerUp();
    return true;
}

void setup() {
    Serial.begin(115200);
//    waitingUsbLoop(); // wait for serial connection
    Serial.println("Begin");
    initRadio();
    Serial.println("Press enter to begin...");
//    waitingUsbInputLoop();
    Serial.flush();
    Serial.println("Starting");
}

char buffer[maxBufferSize];

void processCommand() {
    String cmd(buffer);
    cmd.replace("$", "");
    cmd.trim();
    cmd.toLowerCase();
    if (cmd == "details") {
        Serial.println("Radio Details:");
        radio.printDetails();
    }
}

void RFsend(const String &message) {
    radio.stopListening();
    uint16_t l = message.length();
    uint16_t idx = 0;
    while (l - idx > 31) {
        radio.write(message.substring(idx, idx + 32).c_str(), 32);
        idx += 32;
    }
    radio.write(message.substring(idx).c_str(), l - idx);
    //Serial.println(">>> [" + String(l) + "] '" + message + "'");
    radio.startListening();
}

void loop() {
    // transmit from serial to radio
    if (Serial.available()) {
        uint16_t index = 0;
        while (Serial.available() && index < maxBufferSize)
            buffer[index++] = Serial.read();
        buffer[index] = '\0';
        String str(buffer);
        str.trim();
        if (str.length() > 0) {
            if (buffer[0] == '$') {
                // this is an internal command
                processCommand();
            } else {
                // transmit to other device
                RFsend(str);
            }
        }
    }

    // transmit from radio, to serial
    if (radio.available()) {
        uint8_t index = 0, idx = 32;
        while (idx == 32) {
            idx = radio.getDynamicPayloadSize();
            radio.read(&buffer[index], idx);
            index += idx;
        }
        buffer[index] = '\0';
        //Serial.println("<<< [" + String(index) + "] '" + buffer + "'");
        Serial.println(buffer);
        if (buffer[0] == '$') {
            // this is an internal command
            processCommand();
        }
    }
}
