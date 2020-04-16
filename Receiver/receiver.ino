#include <Arduino.h>
#include <RF24.h>

/**
 * \brief the radio module
 */

const byte device[] = "OnBoarding";
const byte bridge[] = "OnBoardingBridge";
constexpr uint16_t maxBufferSize = 250;

constexpr uint32_t total = 1000;
RF24 radio(10,9);

void failingLoop(){
    constexpr uint32_t wait = total / 8;
    while(1) {
        delay(wait);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(wait);
        digitalWrite(LED_BUILTIN,LOW);
    }
}

void waitingUsbLoop(){
    constexpr uint32_t wait1 = total / 4;
    constexpr uint32_t wait2 = wait1 / 2;
    while(!Serial) {
        delay(wait1 + wait2);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(wait1);
        digitalWrite(LED_BUILTIN,LOW);
        delay(wait2);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(wait1);
        digitalWrite(LED_BUILTIN,LOW);
    }
}

void waitingUsbInputLoop(){
    constexpr uint32_t wait1 = total / 4;
    constexpr uint32_t wait2 = wait1 / 2;
    while(!Serial.available()) {
        delay(wait1 + wait2);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(wait1);
        digitalWrite(LED_BUILTIN,LOW);
        delay(wait2);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(wait1);
        digitalWrite(LED_BUILTIN,LOW);
    }
}

bool initRadio(){
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
    radio.setPALevel(RF24_PA_MAX);
    // Set the transmission data rate (Higher rate reduce range)
    // RF24_250KBPS    RF24_1MBPS    RF24_2MBPS
    radio.setDataRate(RF24_1MBPS);
    // ensure the auto acknowledgment is activated
    radio.setAutoAck(true);
    // define delay & number of retries
    radio.setRetries(2,15);
    // set the CRC length (8-bits for performances)
    // possible: RF24_CRC_DISABLED         RF24_CRC_8          RF24_CRC_16
    //         only if autoAck disabled       8bits               16bits
    radio.setCRCLength(RF24_CRC_8);

    // defining pipes
    radio.openWritingPipe((uint8_t*)"Receiver4586");             // My name (for output)
    radio.openReadingPipe(1,(uint8_t*) "OnBoarding4586"); // name of the other device (listen to)

    // starting all
    radio.startListening();
    radio.powerUp();
}

void setup() {
    Serial.begin(115200);
    waitingUsbLoop(); // wait for serial connection
    Serial.println("Begin");
    initRadio();
    Serial.println("Press enter to begin...");
    waitingUsbInputLoop();
    Serial.flush();
    Serial.println("Starting");
}

char buffer[maxBufferSize];

void processCommand(){
    String cmd(buffer);
    cmd.replace("$","");
    cmd.trim();
    cmd.toLowerCase();
    if (cmd == "details") {
        Serial.println("Radio Details:");
        radio.printDetails();
    }
}

void loop() {
    // transmit from serial to radio
    if (Serial.available()){
        uint16_t index = 0;
        while (Serial.available() && index < maxBufferSize)
            buffer[index++] = Serial.read();
        //Serial.println("buffer size to send: " + String(index));
        buffer[index] = '\0';
        if (index > 0) {
            if (buffer[0] == '$') {
                // this is an internal command
                processCommand();
            }else{
                // transmit to other device
                Serial.print(">>> ");
                Serial.println(buffer);
                radio.write(buffer,index +1);
            }
        }
    }

    // transmit from radio, to serial
    if (radio.available()){
        uint16_t index = 0;
        while (radio.available() && index < maxBufferSize)
            radio.read(&buffer[index++],1);
        radio.flush_rx();
        Serial.println("buffer size received: " + String(index));
        buffer[index] = '\0';
        if (index > 0) {
            if (buffer[0] == '$') {
                // this is an internal command
                processCommand();
            }else {
                Serial.print("<<< '");
                Serial.print(buffer);
                Serial.println("'");
            }
        }
    }
}
