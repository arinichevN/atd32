#ifndef RTC_MAIN_H
#define RTC_MAIN_H

#include "../../lib/common.h"
#include "DS3231.h"

typedef struct {
	void (*adjust)(const DateTime *);
	DateTime *(*now)();
	void (*setDate)(unsigned long);
	void (*setTime)(unsigned long);
} RTC;

extern RTC rtc;

extern int rtc_begin(int device_kind, int default_btn);

#endif 
