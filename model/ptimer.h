#ifndef MODEL_PTIMER_H
#define MODEL_PTIMER_H

#include "../util/common.h"


typedef struct ptimer_st{
	unsigned long interval_on;
	unsigned long interval_off;
	void *data;
	void (*func)(void *, int);
	Ton tmr;
	void (*control)(struct ptimer_st *);
} PTimer;

#endif 
