/*
 * Base station using the ESP8266 and a rf24 module
 * Hackaday io: https://hackaday.io/project/16724-lp-mini
 */

#include "config.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(2, 15);
const byte thisSlaveAddress[] = "RxAAA";
float dataReceived; // this must match dataToSend in the TX

AdafruitIO_Feed *temperature = io.feed("chTemperature");

int temp = 0;

void setup() {
  
  Serial.begin(9600);
  while(! Serial);
  Serial.println("\n\nConnecting to Adafruit IO");
  io.connect();
  
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, thisSlaveAddress);
  radio.startListening();

  int counter = 0;
  while(io.status() < AIO_CONNECTED) {
    delay(100);
    counter++;
    if(counter > 50) ESP.deepSleep(60 * 1000000); // reset if connection fails
  }
  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {
  io.run();
  if (radio.available()){
    radio.read(&dataReceived, sizeof(float));
    Serial.print("Data received: ");
    Serial.println(dataReceived);

    Serial.print("Uploading...");
    temperature->save(dataReceived);
  }
}
