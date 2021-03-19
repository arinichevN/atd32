#ifndef DS3231_H
#define DS3231_H

#include <Wire.h>
#include "date_time.h"

#define DS3231_ADDRESS			0x68
#define DS3231_TIME				0x00
#define DS3231_ALARM1			0x07
#define DS3231_ALARM2			0x0B
#define DS3231_CONTROL			0x0E 
#define DS3231_STATUSREG		0x0F 
#define DS3231_TEMPERATUREREG	0x11

extern int ds3231_begin();
extern int ds3231_powerLost(void);
extern void ds3231_adjust(const DateTime *v);
extern DateTime *ds3231_now();
extern void ds3231_setDate (unsigned long v);
extern void ds3231_setTime (unsigned long v);

#endif
