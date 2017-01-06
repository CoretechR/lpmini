#include <lpmini.h>

uint8_t dectobcd(const uint8_t val){  // Convert Decimal to Binary Coded Decimal
	return ((val/10*16)+(val%10));
}

uint8_t bcdtodec(const uint8_t val){  // Convert Binary Coded Decimal to Decimal
	return ((val/16*10)+(val%16));
}
/*
* LP Mini Library
* Date: 18-11-2016
* Author: Maximilian Kern
* Hackaday io: https://hackaday.io/project/16724-lp-mini
*/

void lpmini_getTime(struct time *t){ // read time from RTC
      
	Wire.beginTransmission(mcp79410_ADD);
	Wire.write(0x00);
	Wire.endTransmission();

	Wire.requestFrom(mcp79410_ADD, 7);
  
	t->sec     = bcdtodec(Wire.read() & 0x7f);  // 01111111
	t->min     = bcdtodec(Wire.read() & 0x7f);  // 01111111
	t->hour    = bcdtodec(Wire.read() & 0x3f);  // 00111111
	t->wday    = bcdtodec(Wire.read() & 0x07);  // 01111111
	t->mday    = bcdtodec(Wire.read() & 0x3f);  // 00111111
	t->month   = bcdtodec(Wire.read() & 0x1f);  // 00011111
	t->year    = bcdtodec(Wire.read());         // 11111111

}

void lpmini_setTime(struct time *t){ // set time for RTC

	Wire.beginTransmission(mcp79410_ADD);
	Wire.write(0x00);
		
	Wire.write(0x80 | (dectobcd(t->sec) & 0x7f)); // set seconds and start clock (01111111)
	Wire.write(dectobcd(t->min) & 0x7f);          // set minutes (01111111)
	Wire.write(dectobcd(t->hour) & 0x3f);         // set hours and to 24hr clock (00111111)
	Wire.write(dectobcd(t->wday) & 0x07);         // set the day (00001000)
	Wire.write(dectobcd(t->mday) & 0x3f);         // set the date in month (00111111)
	Wire.write(dectobcd(t->month) & 0x1f);        // set the month (00011111)
	Wire.write(dectobcd(t->year));                // set the year (11111111)

	Wire.endTransmission();
}

void lpmini_setAlm0(){
  
	Wire.beginTransmission(mcp79410_ADD);
	Wire.write(0x0A); // Alm0 Reg
	Wire.write(dectobcd(45) & 0x7f);
	Wire.endTransmission();

	Wire.beginTransmission(mcp79410_ADD);
	Wire.write(0x0D); // Alm0 Mask Reg
	Wire.write(0b00000000);
	Wire.endTransmission();

	Wire.beginTransmission(mcp79410_ADD);
	Wire.write(0x07); // Control Reg
	Wire.write(0b00010000); // enable Alm0
	Wire.endTransmission();
  
}

void lpmini_resetAlm0(){
  
	Wire.beginTransmission(mcp79410_ADD);
	Wire.write(0x07); // Control Reg
	Wire.write(0b00010000); // enable Alm0
	Wire.endTransmission();
  
}

void lpmini_delay_s(unsigned int seconds){ // 1-60 sec

	time tn;
	lpmini_getTime(&tn);
	seconds += tn.sec;
	if(seconds>=60) seconds -= 60;
  
	Wire.beginTransmission(mcp79410_ADD);
	Wire.write(0x0A); // Alm0 Reg
	Wire.write(dectobcd(seconds) & 0x7f);
	Wire.endTransmission();

	Wire.beginTransmission(mcp79410_ADD);
	Wire.write(0x0D); // Alm0 Mask Reg
	Wire.write(0b00000000);
	Wire.endTransmission();

	Wire.beginTransmission(mcp79410_ADD);
	Wire.write(0x07); // Control Reg
	Wire.write(0b00010000); // enable Alm0
	Wire.endTransmission();
}

void lpmini_delay_min(unsigned int minutes){ // 1-60 min

	time tn;
	lpmini_getTime(&tn);
	minutes += tn.min;
	if(minutes>=60) minutes -= 60;
  
	Wire.beginTransmission(mcp79410_ADD);
	Wire.write(0x0B); // Alm0 Reg
	Wire.write(dectobcd(minutes) & 0x7f);
	Wire.endTransmission();

	Wire.beginTransmission(mcp79410_ADD);
	Wire.write(0x0D); // Alm0 Mask Reg
	Wire.write(0b00010000); // minutes match
	Wire.endTransmission();

	Wire.beginTransmission(mcp79410_ADD);
	Wire.write(0x07); // Control Reg
	Wire.write(0b00010000); // enable Alm0
	Wire.endTransmission();
}

void lpmini_init(){
	
	Wire.begin();
}