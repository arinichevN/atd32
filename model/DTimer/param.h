#ifndef MODEL_DTIMER_PARAM_H
#define MODEL_DTIMER_PARAM_H

#include "../../lib/common.h"

typedef struct {
	unsigned long time_on_s;
	unsigned long time_off_s;
} DTimerParam;

extern err_t dtimerParam_check(const DTimerParam *self);

#endif 
