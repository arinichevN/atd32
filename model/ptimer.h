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

extern void ptimer_begin(PTimer *item);
extern void ptimer_reset(PTimer *item);
extern void ptimer_setParam(PTimer *item, void *data, void (*func)(void *, int),  unsigned long interval_on_s, unsigned long interval_off_s);
extern int ptimer_checkParam(unsigned long interval_on_s, unsigned long interval_off_s);
extern int ptimer_getOutput(PTimer *item);

#endif 
