/*
 * Very basic logger example
 * 
 * This uses an LP mini with 5V regulator, SPI SD module and ds18b20 temperature sensor
*/
#include <LowPower.h> // https://github.com/rocketscream/Low-Power
#include <lpmini.h>
#include <OneWire.h>
#include <DallasTemperature.h> // https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <SD.h>

#define LED 13
#define MOSFET 10
#define ONE_WIRE_BUS 8
#define CHIP_SELECT 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

boolean wake = false;
float temp = 0;

void setup(void){
  pinMode(LED, OUTPUT);
  pinMode(2, INPUT); // interrupt pin as input
  pinMode(MOSFET, OUTPUT);
  digitalWrite(MOSFET, LOW); // LOW means power is enabled on pin 10

  Serial.begin(9600);
  sensors.begin();
  Wire.begin();
  attachInterrupt(0, interruptHandler, FALLING); // RTC Interrupt  
  
  time tn = {0, 32, 18, 2, 10, 12, 16}; // sec,min,hour,weekday,day,month,year
  lpmini_setTime(&tn); // set time in RTC
  delay(100);
  
}

void loop(void){
  if(wake){ // deal with stuff that shouldn't be in the interrupt handler
    lpmini_resetAlm0(); // reset Alarm
    Serial.println("RTC Interrupt!");
    wake = false; // reset flag
  }
  
  time tn;
  lpmini_getTime(&tn); // receive time from RTC
  String timeString = String(tn.hour)+":"+String(tn.min)+":"+String(tn.sec)+" "+String(tn.mday)+"."+String(tn.month)+"."+String(tn.year);
  
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0); // get temperature from ds18b20

  SD.begin(CHIP_SELECT);
  String filename = "lg" + String(tn.mday) + String(tn.month) + String(tn.year) + ".txt"; // filename restricted to 8 chars
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    String dataString = timeString + "  " + String(temp) + "°C ";
    Serial.println(dataString);
    dataFile.println(dataString);
    dataFile.close();
  }
  else Serial.println("error reading SD");

  digitalWrite(MOSFET, HIGH); // disable external power
  pinMode(4, INPUT); // set SPI pins as input to prevent current drain
  pinMode(11, INPUT);
  pinMode(13, INPUT);
  delay(50);
  
  lpmini_delay(30); // set alarm to 30 seconds (works only up to 60 for the moment)
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  digitalWrite(MOSFET, LOW); // enable external power
  pinMode(4, OUTPUT); // enable SPI pins again
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  delay(50);
}

void interruptHandler(){
  wake = true; // set flag
}
