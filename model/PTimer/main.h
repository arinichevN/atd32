#ifndef MODEL_PTIMER_H
#define MODEL_PTIMER_H

#include "../../lib/common.h"
#include "../interface/iSlave.h"
#include "param.h"

typedef struct ptimer_st PTimer;
struct ptimer_st{
	unsigned long interval_on;
	unsigned long interval_off;
	iSlave *slave;
	Ton tmr;
	void (*control)(PTimer *);
};

extern void ptimer_begin(PTimer *self);
extern void ptimer_reset(PTimer *self);
extern void ptimer_setParam(PTimer *self, iSlave *slave, const PTimerParam *param);
extern onf_t ptimer_getOutput(PTimer *self);
extern unsigned int ptimer_getTimeRest(PTimer *self);

#endif 
