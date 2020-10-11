#include "ptimer.h"

void ptimer_INIT(PTimer *item);
void ptimer_WAIT_ON(PTimer *item);
void ptimer_WAIT_OFF(PTimer *item);
void ptimer_ON(PTimer *item);
void ptimer_OFF(PTimer *item);

void ptimer_funcIdle(void *data, int state){
	printd("ptimer: idle function "); printdln(state);
}

void ptimer_setParam(PTimer *item, void *data, void (*func)(void *, int),  unsigned long interval_on_s, unsigned long interval_off_s){
	item->interval_on = interval_on_s;
	item->interval_off = interval_off_s;
	item->data = data;
	item->func = func;
	if(item->func == NULL){
		item->func = ptimer_funcIdle;
	}
}

void ptimer_begin(PTimer *item){
	item->control = ptimer_INIT;
}

int ptimer_checkParam(unsigned long interval_on_s, unsigned long interval_off_s){
	return ERROR_NO;
}


void ptimer_reset(PTimer *item){
	item->control = ptimer_INIT;
}

int ptimer_getOutput(PTimer *item){
	if(item->control == ptimer_WAIT_OFF || item->control == ptimer_ON) return ON;
	return OFF;
}

void ptimer_setOutput(PTimer *item){
	item->func(item->data, ptimer_getOutput(item));
}

void ptimer_INIT(PTimer *item){
	if(item->interval_on == 0){
		item->control = ptimer_OFF;
	}else if(item->interval_off == 0){
		item->control = ptimer_ON;
	}else{
		ton_setInterval(&item->tmr, item->interval_on * MILLISECONDS_IN_SECOND);
		ton_reset(&item->tmr);
		item->control = ptimer_WAIT_OFF;
	}
	ptimer_setOutput(item);
}

void ptimer_WAIT_ON(PTimer *item){
	if(ton(&item->tmr)){
		ton_setInterval(&item->tmr, item->interval_on * MILLISECONDS_IN_SECOND);
		ton_reset(&item->tmr);
		item->control = ptimer_WAIT_OFF;
		ptimer_setOutput(item);
	}
}

void ptimer_WAIT_OFF(PTimer *item){
	if(ton(&item->tmr)){
		ton_setInterval(&item->tmr, item->interval_off * MILLISECONDS_IN_SECOND);
		ton_reset(&item->tmr);
		item->control = ptimer_WAIT_ON;
		ptimer_setOutput(item);
	}
}

void ptimer_ON(PTimer *item){
	;
}

void ptimer_OFF(PTimer *item){
	;
}

unsigned int ptimer_getTimeRest(PTimer *item){
	if(item->control == ptimer_WAIT_ON || item->control == ptimer_WAIT_OFF){
		return ton_getRest(&item->tmr);
	}
	return 0;
}
