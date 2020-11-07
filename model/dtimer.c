#include "dtimer.h"

void dtimer_INIT(DTimer *item);

void dtimer_IDLE(DTimer *item);

void dtimer_BUSY(DTimer *item);

void dtimer_OFF(DTimer *item);

void dtimer_FAILURE(DTimer *item);

static unsigned long dateTimeToTimeOfDayS (const DateTime *v){
	unsigned long h = (unsigned long) v->hh;
	unsigned long m = (unsigned long) v->mm;
	unsigned long s = (unsigned long) v->ss;
	unsigned long r = h*SECONDS_IN_HOUR + m*SECONDS_IN_MINUTE + s;
	printd("tod, s: ");printdln(r);
	return r;
}

static int getTimeOfDayS(unsigned long *out){
	extern RTC rtc;
	DateTime *ndt = rtc.now();
	if(ndt == NULL) return 0;
	*out = dateTimeToTimeOfDayS(ndt);
	return 1;
}

static int calcState (unsigned long t1,  unsigned long t2, unsigned long t){
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

static void setStateById (DTimer *item, int state_id){
	switch(state_id){
		case IDLE: item->control = dtimer_IDLE; return;
		case BUSY: item->control = dtimer_BUSY; return;
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

int dtimer_checkParam(unsigned long time_on_s, unsigned long time_off_s){
	if(time_on_s > SECONDS_IN_DAY){
		printdln("time_on is over");
		return ERROR_SOME;
	}
	if(time_off_s > SECONDS_IN_DAY){
		printdln("time_off is over");
		return ERROR_SOME;
	}
	if(time_on_s == time_off_s){
		printdln("time_on == time_off");
		return ERROR_SOME;
	}
	return ERROR_NO;
}

int dtimer_getOutput(DTimer *item){
	if(item->control == dtimer_BUSY) return ON;
	return OFF;
}

void dtimer_setParam(DTimer *item, unsigned long time_on_s, unsigned long time_off_s){
	item->time_on = time_on_s;
	item->time_off = time_off_s;
	printd(" "); printd(item->time_on); printd(" - "); printdln(item->time_off);
}

void dtimer_begin(DTimer *item){
	item->control = dtimer_INIT;
}
#define PRINTINFO(ITEM) printd("go to "); printd(dtimer_getStateStr(ITEM)); printd("; time rest, s: "); printdln(dtimer_getTimeRest(ITEM)/MILLISECONDS_IN_SECOND);
#define TON_PREP(ITEM, NOW) ton_setInterval(&(ITEM)->tmr, calcIntervalMS((ITEM)->time_on, (ITEM)->time_off, NOW));	ton_reset(&(ITEM)->tmr);

void dtimer_INIT(DTimer *item){
	unsigned long t;
	if(!getTimeOfDayS(&t)){
		item->control = dtimer_FAILURE;
		PRINTINFO(item)
		return;
	}
	setStateById(item, calcState(item->time_on, item->time_off, t));
	TON_PREP(item, t)
	PRINTINFO(item)
}

void dtimer_IDLE(DTimer *item){
	if(ton(&item->tmr)){
		unsigned long t;
		if(!getTimeOfDayS(&t)){
			item->control = dtimer_FAILURE;
			PRINTINFO(item)
			return;
		}
		if(calcState(item->time_on, item->time_off, t) == BUSY){
			TON_PREP(item, t)
			item->control = dtimer_BUSY;
			PRINTINFO(item)
		}
	}
}

void dtimer_BUSY(DTimer *item){
	if(ton(&item->tmr)){
		unsigned long t;
		if(!getTimeOfDayS(&t)){
			item->control = dtimer_FAILURE;
			PRINTINFO(item)
			return;
		}
		if(calcState(item->time_on, item->time_off, t) == IDLE){
			TON_PREP(item, t)
			item->control = dtimer_IDLE;
			PRINTINFO(item)
		}
	}
}

void dtimer_OFF(DTimer *item){
	;
}

void dtimer_FAILURE(DTimer *item){
	;
}

unsigned long dtimer_getTimeRest(DTimer *item){
	if(item->control == dtimer_IDLE || item->control == dtimer_BUSY){
		return ton_getRest(&item->tmr);
	}
	return 0;
}

const char *dtimer_getStateStr(DTimer *item){
	if(item->control == dtimer_IDLE)			return "IDLE";
	else if(item->control == dtimer_BUSY)		return "BUSY";
	else if(item->control == dtimer_OFF)		return "OFF";
	else if(item->control == dtimer_FAILURE)	return "FAILURE";
	else if(item->control == dtimer_INIT)		return "INIT";
	return "?";
}

int dtimer_getState(DTimer *item){
	if(item->control == dtimer_IDLE)			return IDLE;
	else if(item->control == dtimer_BUSY)		return BUSY;
	else if(item->control == dtimer_OFF)		return OFF;
	else if(item->control == dtimer_FAILURE)	return FAILURE;
	else if(item->control == dtimer_INIT)		return INIT;
	return -1;
}
