#ifndef MODEL_DTIMER_H
#define MODEL_DTIMER_H

#include "../../lib/common.h"
#include "../../lib/ton.h"
#include "../../app/rtc/main.h"
#include "param.h"

typedef struct dtimer_st DTimer;
struct dtimer_st{
	unsigned long time_on;
	unsigned long time_off;
	Ton tmr;
	void (*control) (DTimer *);
};

typedef void (*DTimerControlFunc) (DTimer *);

extern void dtimer_setParam(DTimer *self, const DTimerParam *param);
extern void dtimer_begin(DTimer *self);
extern unsigned long dtimer_getTimeRest(DTimer *self);
extern const char *dtimer_getStateStr(DTimer *self);
extern state_t dtimer_getState(DTimer *self);
extern onf_t dtimer_getOutput(DTimer *self);

#endif 
