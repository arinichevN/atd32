#include "main.h"

static void ptimer_INIT(PTimer *self);
static void ptimer_WAIT_ON(PTimer *self);
static void ptimer_WAIT_OFF(PTimer *self);
static void ptimer_ON(PTimer *self);
static void ptimer_OFF(PTimer *self);

void ptimer_setParam(PTimer *self, iSlave *slave,  const PTimerParam *param){
	self->interval_on = param->interval_on_s;
	self->interval_off = param->interval_off_s;
	self->slave = slave;
}

void ptimer_begin(PTimer *self){
	self->control = ptimer_INIT;
}

void ptimer_reset(PTimer *self){
	self->control = ptimer_INIT;
}

onf_t ptimer_getOutput(PTimer *self){
	if(self->control == ptimer_WAIT_OFF || self->control == ptimer_ON) return ON;
	return OFF;
}

static void ptimer_setOutput(PTimer *self){
	self->slave->perform(self->slave->self, ptimer_getOutput(self));
}

static void ptimer_INIT(PTimer *self){
	if(self->interval_on == 0){
		self->control = ptimer_OFF;
	}else if(self->interval_off == 0){
		self->control = ptimer_ON;
	}else{
		ton_setInterval(&self->tmr, self->interval_on * MILLISECONDS_IN_SECOND);
		ton_reset(&self->tmr);
		self->control = ptimer_WAIT_OFF;
	}
	ptimer_setOutput(self);
}

static void ptimer_WAIT_ON(PTimer *self){
	if(ton(&self->tmr)){
		ton_setInterval(&self->tmr, self->interval_on * MILLISECONDS_IN_SECOND);
		ton_reset(&self->tmr);
		self->control = ptimer_WAIT_OFF;
		ptimer_setOutput(self);
	}
}

static void ptimer_WAIT_OFF(PTimer *self){
	if(ton(&self->tmr)){
		ton_setInterval(&self->tmr, self->interval_off * MILLISECONDS_IN_SECOND);
		ton_reset(&self->tmr);
		self->control = ptimer_WAIT_ON;
		ptimer_setOutput(self);
	}
}

static void ptimer_ON(PTimer *self){
	;
}

static void ptimer_OFF(PTimer *self){
	;
}

unsigned int ptimer_getTimeRest(PTimer *self){
	if(self->control == ptimer_WAIT_ON || self->control == ptimer_WAIT_OFF){
		return ton_getRest(&self->tmr);
	}
	return 0;
}
