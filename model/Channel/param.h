#ifndef MODEL_CHANNEL_PARAM_H
#define MODEL_CHANNEL_PARAM_H

#include "../../lib/common.h"
#include "../DTimer/param.h"
#include "../PTimer/param.h"

typedef struct {
	int id;
	int pin;
	int initial_pin_state;
	yn_t enable;
	DTimerParam dtimer;
	PTimerParam ptimer;
} ChannelParam;

extern err_t channelParam_check(const ChannelParam *self);

#endif 
