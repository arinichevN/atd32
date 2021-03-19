#ifndef MODEL_CHANNEL_AOID_H
#define MODEL_CHANNEL_AOID_H

#include "../../lib/common.h"

typedef struct {
	Aoid main;
	Aoid time_on;
	Aoid time_off;
} DTimerAoid;

typedef struct {
	Aoid main;
	Aoid interval_on;
	Aoid interval_off;
} PTimerAoid;

typedef struct {
	Aoid main;
	Aoid id;
	Aoid pin;
	Aoid initial_pin_state;
	Aoid enable;
	DTimerAoid dtimer;
	PTimerAoid ptimer;
} ChannelAoid;

extern void channelAoid_build(ChannelAoid *self, Aoid *next, Aoid *parent, size_t *id, void *vchannel);

#endif 
