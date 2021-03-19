#include "main.h"

void dtimer_INIT(DTimer *self);
void dtimer_IDLE(DTimer *self);
void dtimer_BUSY(DTimer *self);
void dtimer_OFF(DTimer *self);
void dtimer_FAILURE(DTimer *self);

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

static DTimerControlFunc calcState (unsigned long t1,  unsigned long t2, unsigned long t){
	if(t1 < t2){
		if(t >= t1 && t < t2){
			return dtimer_BUSY;
		}
		return dtimer_IDLE;
	}
	if(t >= t1 || t < t2){
		return dtimer_BUSY;
	}
	return dtimer_IDLE;
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

onf_t dtimer_getOutput(DTimer *self){
	if(self->control == dtimer_BUSY) return ON;
	return OFF;
}

void dtimer_setParam(DTimer *self, const DTimerParam *param){
	self->time_on = param->time_on_s;
	self->time_off = param->time_off_s;
	printd(" "); printd(self->time_on); printd(" - "); printdln(self->time_off);
}

void dtimer_begin(DTimer *self){
	self->control = dtimer_INIT;
}
#define PRINTINFO(ITEM) printd("go to "); printd(dtimer_getStateStr(ITEM)); printd("; time rest, s: "); printdln(dtimer_getTimeRest(ITEM)/MILLISECONDS_IN_SECOND);
#define TON_PREP(ITEM, NOW) ton_setInterval(&(ITEM)->tmr, calcIntervalMS((ITEM)->time_on, (ITEM)->time_off, NOW));	ton_reset(&(ITEM)->tmr);

void dtimer_INIT(DTimer *self){
	unsigned long t;
	if(!getTimeOfDayS(&t)){
		self->control = dtimer_FAILURE;
		PRINTINFO(self)
		return;
	}
	self->control = calcState(self->time_on, self->time_off, t);
	TON_PREP(self, t)
	PRINTINFO(self)
}

void dtimer_IDLE(DTimer *self){
	if(ton(&self->tmr)){
		unsigned long t;
		if(!getTimeOfDayS(&t)){
			self->control = dtimer_FAILURE;
			PRINTINFO(self)
			return;
		}
		if(calcState(self->time_on, self->time_off, t) == dtimer_BUSY){
			TON_PREP(self, t)
			self->control = dtimer_BUSY;
			PRINTINFO(self)
		}
	}
}

void dtimer_BUSY(DTimer *self){
	if(ton(&self->tmr)){
		unsigned long t;
		if(!getTimeOfDayS(&t)){
			self->control = dtimer_FAILURE;
			PRINTINFO(self)
			return;
		}
		if(calcState(self->time_on, self->time_off, t) == dtimer_IDLE){
			TON_PREP(self, t)
			self->control = dtimer_IDLE;
			PRINTINFO(self)
		}
	}
}

void dtimer_OFF(DTimer *self){
	;
}

void dtimer_FAILURE(DTimer *self){
	;
}

unsigned long dtimer_getTimeRest(DTimer *self){
	if(self->control == dtimer_IDLE || self->control == dtimer_BUSY){
		return ton_getRest(&self->tmr);
	}
	return 0;
}

const char *dtimer_getStateStr(DTimer *self){
	if(self->control == dtimer_IDLE)			return "IDLE";
	else if(self->control == dtimer_BUSY)		return "BUSY";
	else if(self->control == dtimer_OFF)		return "OFF";
	else if(self->control == dtimer_FAILURE)	return "FAILURE";
	else if(self->control == dtimer_INIT)		return "INIT";
	return "?";
}

state_t dtimer_getState(DTimer *self){
	if(self->control == dtimer_IDLE)			return STATE_IDLE;
	else if(self->control == dtimer_BUSY)		return STATE_BUSY;
	else if(self->control == dtimer_OFF)		return STATE_OFF;
	else if(self->control == dtimer_INIT)		return STATE_INIT;
	return STATE_FAILURE;
}
