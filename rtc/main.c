#include "main.h"


int rtc_begin(RTC *item, int device_kind, int default_btn){
	item->now = NULL;
	item->adjust = NULL;
	switch(device_kind){
		case DEVICE_KIND_DS3231:
			item->now = &ds3231_now;
			item->adjust = &ds3231_adjust;
			item->setDate = &ds3231_setDate;
			item->setTime = &ds3231_setTime;
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
				return 0;
			}
			return 1;
	}
	return 0;
}
