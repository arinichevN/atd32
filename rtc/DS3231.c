#include "DS3231.h"

#define RTC_BUF _rtc_datetime

static DateTime RTC_BUF;

#define WIRE_END Wire.endTransmission(true)
static uint8_t read_i2c_register(uint8_t addr, uint8_t reg) {
  Wire.beginTransmission(addr);
  Wire.write((uint8_t)reg);
  WIRE_END;

  Wire.requestFrom(addr, (uint8_t)1);
  return Wire.read();
}

static void write_i2c_register(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)val);
  WIRE_END;
}

static uint8_t bcdTobin(uint8_t v) {
	return v - 6 * (v >> 4);
}

static uint8_t binTobcd(uint8_t v) {
	return v + 6 * (v / 10);
}

int ds3231_begin() {
	Wire.begin();
	Wire.beginTransmission(DS3231_ADDRESS);
	if (WIRE_END != 0) return 0;
	return 1;
}

int ds3231_powerLost(void) {
  return (read_i2c_register(DS3231_ADDRESS, DS3231_STATUSREG) >> 7);
}

void ds3231_adjust(const DateTime *v) {
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write((uint8_t)DS3231_TIME);
  Wire.write(binTobcd(v->ss));
  Wire.write(binTobcd(v->mm));
  Wire.write(binTobcd(v->hh));
  Wire.write(binTobcd(0));
  Wire.write(binTobcd(v->d));
  Wire.write(binTobcd(v->m));
  Wire.write(binTobcd((uint8_t)v->y));
  WIRE_END;

  uint8_t statreg = read_i2c_register(DS3231_ADDRESS, DS3231_STATUSREG);
  statreg &= ~0x80;
  write_i2c_register(DS3231_ADDRESS, DS3231_STATUSREG, statreg);
}


DateTime *ds3231_now() {
	Wire.beginTransmission(DS3231_ADDRESS);
	Wire.write((uint8_t)0);
	if(WIRE_END!= 0) {printdln("rtc: failed to read");return NULL;}
	DateTime *dt = &RTC_BUF;
	Wire.requestFrom(DS3231_ADDRESS, 7);
	dt->ss = bcdTobin(Wire.read() & 0x7F);
	dt->mm = bcdTobin(Wire.read());
	dt->hh = bcdTobin(Wire.read());
	Wire.read();
	dt->d = bcdTobin(Wire.read());
	dt->m = bcdTobin(Wire.read());
	dt->y = bcdTobin(Wire.read());
	printd("rtc: now: ");printd(dt->y); printd("-");printd(dt->m); printd("-");printdln(dt->d);printd(" ");printd(dt->hh); printd(":");printd(dt->mm); printd(":");printdln(dt->ss);
	return dt;
}

void ds3231_setDate (unsigned long v){
	unsigned long y = v / DAYS_IN_YEAR;
	unsigned long m = (v % DAYS_IN_YEAR) / DAYS_IN_MONTH;
	unsigned long d = v % DAYS_IN_MONTH;
	if(y < 0 || y > 99) return;
	if(m < 1 || m > 12) return;
	if(d < 1 || d > 31) return;
	DateTime *ndt = ds3231_now();
	DateTime dt = *ndt;
	dt.y = y;
	dt.m = m;
	dt.d = d;
	printd("rtc: set date: ");printd(dt.y); printd("-");printd(dt.m); printd("-");printdln(dt.m);
	ds3231_adjust(&dt);
}

void ds3231_setTime (unsigned long v){
	unsigned long hh = v / SECONDS_IN_HOUR;
	unsigned long mm = (v % SECONDS_IN_HOUR) / SECONDS_IN_MINUTE;
	unsigned long ss = v % SECONDS_IN_MINUTE;
	if(hh < 0 || hh > 23) return;
	if(mm < 0 || mm > 59) return;
	if(ss < 0 || ss > 59) return;
	DateTime *ndt = ds3231_now();
	DateTime dt = *ndt;
	dt.hh = hh;
	dt.mm = mm;
	dt.ss = ss;
	printd("rtc: set time: ");printd(dt.hh); printd(":");printd(dt.mm); printd(":");printdln(dt.ss);
	ds3231_adjust(&dt);
}
