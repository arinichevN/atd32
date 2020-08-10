#include "ptimer.h"

void ptimer_setParam(PTimer *item, void *data, void (*func)(void *, int),  unsigned long interval_on_s, unsigned long interval_off_s){
	item->interval_on = interval_on_s;
	item->interval_off = interval_off_s;
	item->data = data;
	item->func = func;
}

void ptimer_begin(PTimer *item){
	item->state = INIT;
}

int ptimer_check(PTimer *item){
	return ERROR_NO;
}


void ptimer_reset(PTimer *item){
	item->state = INIT;
}

int ptimer_getOutput(PTimer *item){
	switch(item->state){
		case WAIT_OFF:
		case ON:
			return ON;
	}
	return OFF;
}

void ptimer_setOutput(PTimer *item){
	int state = OFF;
	switch(item->state){
		case WAIT_OFF:
		case ON: state = ON;
	}
	if(item->func != NULL){
		item->func(item->data, state);
	}
}

int ptimer_control(PTimer *item){
	switch(item->state){
		case WAIT_ON:
			if(ton(&item->tmr)){
				ton_setInterval(&item->tmr, item->interval_on * MILLISECONDS_IN_SECOND);
				ton_reset(&item->tmr);
				item->state = WAIT_OFF;
				ptimer_setOutput(item);
			}
			break;
		case WAIT_OFF:
			if(ton(&item->tmr)){
				ton_setInterval(&item->tmr, item->interval_off * MILLISECONDS_IN_SECOND);
				ton_reset(&item->tmr);
				item->state = WAIT_ON;
				ptimer_setOutput(item);
			}
			break;
		case ON: break;
		case OFF: break;
		case INIT:
			if(item->interval_on == 0){
				item->state = OFF;
			}else if(item->interval_off == 0){
				item->state = ON;
			}else{
				ton_setInterval(&item->tmr, item->interval_on * MILLISECONDS_IN_SECOND);
				ton_reset(&item->tmr);
				item->state = WAIT_OFF;
			}
			ptimer_setOutput(item);
			break;
	}
	return item->state;
}

unsigned int ptimer_getTimeRest(PTimer *item){
	if(item->state == WAIT_ON || item->state == WAIT_OFF){
		return ton_getRest(&item->tmr);
	}
	return 0;
}
