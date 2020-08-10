#include "dtimer.h"

static unsigned long dateTimeToTimeOfDayS (const DateTime *v){
	unsigned long h = (unsigned long) v->hh;
	unsigned long m = (unsigned long) v->mm;
	unsigned long s = (unsigned long) v->ss;
	unsigned long r = h*SECONDS_IN_HOUR + m*SECONDS_IN_MINUTE + s;
	printd("tod, s: ");printdln(r);
	return r;
}

static int getTimeOfDayS(RTC *rtc, unsigned long *out){
	DateTime *ndt = rtc->now();
	if(ndt == NULL) return 0;
	*out = dateTimeToTimeOfDayS(ndt);
	return 1;
}

static int calcState(unsigned long t1,  unsigned long t2, unsigned long t){
	if(t1 < t2){
		if(t >= t1 && t < t2){
			return BUSY;
		}else{
			return IDLE;
		}
	}else{
		if(t >= t1 || t < t2){
			return BUSY;
		}else{
			return IDLE;
		}
	}
	
}

static unsigned long calcIntervalMS(unsigned long t1,  unsigned long t2, unsigned long t){
	unsigned long t_min = t1;
	unsigned long t_max = t2;
	if(t2 < t1) {
		t_min = t2;
		t_max = t1;
	}
	if(t >= t_min && t < t_max){
		return (t_max - t)*MILLISECONDS_IN_SECOND;
	}else{
		if(t < t_min){
			return (t_min - t)*MILLISECONDS_IN_SECOND;
		} else {//t >= t_max
			return (SECONDS_IN_DAY - t + t_min)*MILLISECONDS_IN_SECOND;
		}
	}
	return 0;
}

int dtimer_check(DTimer *item){
	if(item->time_on > SECONDS_IN_DAY){
		printdln("time_on is over");
		return ERROR_SOME;
	}
	if(item->time_off > SECONDS_IN_DAY){
		printdln("time_off is over");
		return ERROR_SOME;
	}
	if(item->time_on == item->time_off){
		printdln("time_on == time_off");
		return ERROR_SOME;
	}
	return ERROR_NO;
}

int dtimer_getOutput(DTimer *item){
	switch(item->state){
		case BUSY: return ON;
	}
	return OFF;
}

void dtimer_setParam(DTimer *item, unsigned long time_on_s, unsigned long time_off_s){
	item->time_on = time_on_s;
	item->time_off = time_off_s;
	printd(" "); printd(item->time_on); printd(" - "); printdln(item->time_off);
}

void dtimer_begin(DTimer *item){
	item->state = INIT;
}

#define PRINTINFO(ITEM) printd("go to "); printd(dtimer_getStateStr(ITEM)); printd("; time rest, s: "); printdln(dtimer_getTimeRest(ITEM)/MILLISECONDS_IN_SECOND);
#define TON_PREP(ITEM, NOW) ton_setInterval(&(ITEM)->tmr, calcIntervalMS((ITEM)->time_on, (ITEM)->time_off, NOW));	ton_reset(&(ITEM)->tmr);

int dtimer_control(DTimer *item, RTC *rtc){
	switch(item->state){
		case IDLE:
			if(ton(&item->tmr)){
				unsigned long t;
				if(!getTimeOfDayS(rtc, &t)){
					item->state = FAILURE;
					PRINTINFO(item)
					break;
				}
				if(calcState(item->time_on, item->time_off, t) == BUSY){
					TON_PREP(item, t)
					item->state = BUSY;
					PRINTINFO(item)
				}
			}
			break;
		case BUSY:
			if(ton(&item->tmr)){
				unsigned long t;
				if(!getTimeOfDayS(rtc, &t)){
					item->state = FAILURE;
					PRINTINFO(item)
					break;
				}
				if(calcState(item->time_on, item->time_off, t) == IDLE){
					TON_PREP(item, t)
					item->state = IDLE;
					PRINTINFO(item)
				}
			}
			break;
		case OFF:
			break;
		case FAILURE:
			break;
		case INIT:{
			unsigned long t;
			if(!getTimeOfDayS(rtc, &t)){
				item->state = FAILURE;
				PRINTINFO(item)
				break;
			}
			item->state = calcState(item->time_on, item->time_off, t);
			TON_PREP(item, t)
			}
			PRINTINFO(item)
			break;
	}
	return item->state;
}

unsigned long dtimer_getTimeRest(DTimer *item){
	if(item->state == IDLE || item->state == BUSY){
		return ton_getRest(&item->tmr);
	}
	return 0;
}

const char *dtimer_getStateStr(DTimer *item){
	switch(item->state){
		case IDLE: return "IDLE";
		case BUSY: return "BUSY";
		case OFF:  return "OFF";
		case FAILURE: return "FAILURE";
		case INIT: return "INIT";
	}
	return "?";
}
