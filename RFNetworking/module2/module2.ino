#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

RF24 radio(9, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002", "00003", "00004"};
char message[32];

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(addresses[2]);    // 00003
  radio.openReadingPipe(1, addresses[0]); // 00001
  radio.openReadingPipe(2, addresses[1]); // 00002
  radio.openReadingPipe(3, addresses[3]); // 00004
  radio.setPALevel(RF24_PA_MAX);
}

void loop() {
  delay(100);

  radio.startListening();
  if (radio.available()) {
    radio.read(&message, sizeof(message));
    Serial.print("Received: ");
    Serial.println(message);

    bool transmissionSuccess = true;
    radio.stopListening();
    radio.write(&transmissionSuccess, sizeof(transmissionSuccess));
  }
  
  if (Serial.available()) {
    size_t length = Serial.readBytesUntil('\n', message, sizeof(message) - 1);
    message[length] = '\0'; 

    char* trimmedMessage = message;
    while (*trimmedMessage == ' ' || *trimmedMessage == '\t') {
      trimmedMessage++;
    }

    radio.stopListening();
    bool transmissionSuccess = false;
    while (!transmissionSuccess) {
      radio.write(&transmissionSuccess, sizeof(transmissionSuccess)); 
      if (radio.write(trimmedMessage, strlen(trimmedMessage) + 1)) {  
        if (radio.available()) {
          radio.read(&transmissionSuccess, sizeof(transmissionSuccess)); 
        }
      }
    }
    Serial.print("Sent: ");
    Serial.println(trimmedMessage);
  }
}

