#ifndef MODEL_DTIMER_H
#define MODEL_DTIMER_H

#include "../util/common.h"
#include "../util/ton.h"
#include "../rtc/main.h"


typedef struct dtimer_st{
	unsigned long time_on;
	unsigned long time_off;
	Ton tmr;
	void (*control) (struct dtimer_st *);
} DTimer;


extern int dtimer_check(DTimer *item);

extern void dtimer_setParam(DTimer *item, unsigned long hour1, unsigned long minute1, unsigned long second1, unsigned long hour2, unsigned long minute2, unsigned long second2);

extern void dtimer_begin(DTimer *item);

extern unsigned long dtimer_getTimeRest(DTimer *item);

extern const char *dtimer_getStateStr(DTimer *item);

extern int dtimer_getState(DTimer *item);

#endif 
