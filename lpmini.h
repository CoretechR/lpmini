#include <Arduino.h>
#include <Wire.h>

#define mcp79410_ADD 0x6F


struct time{
  unsigned char sec;
  unsigned char min;
  unsigned char hour;
  unsigned char wday;
  unsigned char mday;
  unsigned char month;    
  unsigned int year;
};

uint8_t dectobcd(const uint8_t val);

uint8_t bcdtodec(const uint8_t val);

void lpmini_getTime(struct time *t);

void lpmini_setTime(struct time *t);

void lpmini_setAlm0();

void lpmini_resetAlm0();

void lpmini_delay(unsigned int seconds);

void lpmini_init();