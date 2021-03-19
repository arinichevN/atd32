#include "main.h"

RTC rtc;

int rtc_begin(int device_kind, int default_btn){
	rtc.now = NULL;
	rtc.adjust = NULL;
	switch(device_kind){
		case DEVICE_KIND_DS3231:
			rtc.now = &ds3231_now;
			rtc.adjust = &ds3231_adjust;
			rtc.setDate = &ds3231_setDate;
			rtc.setTime = &ds3231_setTime;
			if (!ds3231_begin()) {
				printdln("RTC: not found");
				return 0;
			}
			if (default_btn == BUTTON_DOWN) {
				/*
				 * -user_config:
				 * you can update RTC date and time here 
				 * {year-2000,month,day, hour,minute,second}:
				 */
				DateTime dt = {20,7,1, 10,0,0};
				
				ds3231_adjust(&dt);
				printdln("RTC: date and time updated");
			}else if (ds3231_powerLost()) {
				printdln("RTC: date and time lost");
				return 1;
			}
			return 1;
	}
	return 0;
}
