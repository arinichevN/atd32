#ifndef MODEL_PTIMER_PARAM_H
#define MODEL_PTIMER_PARAM_H

#include "../../lib/common.h"

typedef struct {
	unsigned long interval_on_s;
	unsigned long interval_off_s;
} PTimerParam;

extern err_t ptimerParam_check(const PTimerParam *self);

#endif 
