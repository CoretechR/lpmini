/*
* This example shows how to configure the LP mini 
* to wake up periodically from the RTC
* Hackaday io: https://hackaday.io/project/16724-lp-mini
*/

#include <LowPower.h> // https://github.com/rocketscream/Low-Power
#include <lpmini.h>

#define LED 13
#define MOSFET 10

boolean wake = false;

void setup(void){
  pinMode(LED, OUTPUT);
  pinMode(MOSFET, OUTPUT);
  pinMode(2, INPUT); // interrupt pin as input
  digitalWrite(MOSFET, HIGH); // HIGH means mosfet is off

  
  Serial.begin(9600);
  Wire.begin();
  attachInterrupt(0, interruptHandler, FALLING); // RTC Interrupt  
  
  time tn = {0, 18, 19, 1, 16, 11, 16}; // sec,min,hour,weekday,day,month,year
  lpmini_setTime(&tn); // set time in RTC
  
}

void loop(void){
  if(wake){
    lpmini_resetAlm0(); // reset Alarm
    Serial.println("\n RTC Interrupt! \n");
    wake = false; // reset flag
  }
  
  time tn;
  lpmini_getTime(&tn); // receive time from RTC
  Serial.print(tn.hour);
  Serial.print(":");
  Serial.print(tn.min);
  Serial.print(":");
  Serial.print(tn.sec);
  Serial.print(" ");
  Serial.print(tn.mday);
  Serial.print(".");
  Serial.print(tn.month);
  Serial.print(".");
  Serial.println(tn.year);
  digitalWrite(MOSFET, LOW);
  delay(100);
  digitalWrite(MOSFET, HIGH);
  lpmini_delay(5); // set alarm
  
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  
}

void interruptHandler(){
  wake = true; // set flag
}


