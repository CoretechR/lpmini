/*
 * Transmitter using LP mini board, rf24 module and ds18b20 sensor
 * Hackaday io: https://hackaday.io/project/16724-lp-mini
 */

#include <LowPower.h> // https://github.com/rocketscream/Low-Power
#include <lpmini.h>
#include <OneWire.h> // https://github.com/PaulStoffregen/OneWire
#include <DallasTemperature.h> // https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define LED 13
#define MOSFET 10
#define ONE_WIRE_BUS 8

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
RF24 radio(6, 7); // Create a Radio

boolean wake = false;
float temp = 0;
const byte slaveAddress[5] = "RxAAA";
//char dataToSend[10];

void setup(void){
  pinMode(LED, OUTPUT);
  pinMode(2, INPUT); // interrupt pin as input
  pinMode(MOSFET, OUTPUT);
  digitalWrite(MOSFET, LOW); // LOW means power is enabled on pin 10

  Serial.begin(9600);
  Serial.println("Start");
  sensors.begin();
  Wire.begin();
  attachInterrupt(0, interruptHandler, FALLING); // RTC Interrupt

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(3,5); // delay, count
  radio.openWritingPipe(slaveAddress);
  
  time tn = {0, 32, 18, 2, 10, 12, 16}; // sec,min,hour,weekday,day,month,year
  lpmini_setTime(&tn); // set time in RTC
  delay(100);
  
}

void loop(void){
  if(wake){ // deal with stuff that shouldn't be in the interrupt handler
    lpmini_resetAlm0(); // reset Alarm
    Serial.println("\nRTC Interrupt!");
    wake = false; // reset flag
  }
  
  time tn;
  lpmini_getTime(&tn); // receive time from RTC
  String timeString = String(tn.hour)+":"+String(tn.min)+":"+String(tn.sec)+" "+String(tn.mday)+"."+String(tn.month)+"."+String(tn.year);
  
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0); // get temperature from ds18b20
  Serial.println(temp);

  Serial.print("Sending Data: ");
  Serial.print(temp);
  radio.write(&temp, sizeof(float));

  radio.powerDown(); // power down rf24
  digitalWrite(MOSFET, HIGH); // disable external power
  delay(50);
  
  lpmini_delay_min(5); // set alarm to 5 minutes (works only up to 60 for the moment)
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  radio.powerUp(); // wake up rf24
  digitalWrite(MOSFET, LOW); // enable external power
  delay(50);
}

void interruptHandler(){
  wake = true; // set flag
}
