#include "main.h"

static void channel_OFF(Channel *self);
static void channel_INIT(Channel *self);
static void channel_FAILURE(Channel *self);
static void channel_RUN(Channel *self);

static void channel_setOutput(void *vself, int state){
	Channel *self = (Channel *) vself;
	printd("set output for channel ");printd(self->id);printd(" pin ");printdln(self->pin);
	if(state == ON){
		digitalWrite(self->pin, self->initial_pin_state); 
		self->output = 1.0;
		return;
	}
	self->output = 0.0;
	digitalWrite(self->pin, self->other_pin_state);
}

state_t channel_getState(Channel *self){
	if(self->control == channel_OFF) {
		return STATE_OFF;
	} else if(self->control == channel_RUN) {
		return STATE_RUN;
	} else if(self->control == channel_INIT) {
		return STATE_INIT;
	} else if(self->control == channel_FAILURE) {
		return STATE_FAILURE;
	}
	return STATE_UNKNOWN;
}

err_t channel_getError(Channel *self){
	return self->error_id;
}

static void channel_buildInterfaces(Channel *self){
	self->im_slave.self = self;
	self->im_slave.perform = channel_setOutput;
}

static err_t channel_setParam(Channel *self, size_t ind){
	self->ind = ind;
	ChannelParam param;
	if(!pmem_getChannelParam(&param, ind)){
		printdln("   failed to get channel from NVRAM");
		return ERROR_NVRAM_READ;
	}
	err_t r = channelParam_check(&param);
	if(r != ERROR_NO){
		return r;
	}
	self->id = param.id;
	self->pin = param.pin;
	self->initial_pin_state = param.initial_pin_state;
	self->enable = param.enable;
	dtimer_setParam(&self->dtimer, &param.dtimer);
	ptimer_setParam(&self->ptimer, &self->im_slave, &param.ptimer);
	return ERROR_NO;
}

void channel_begin(Channel *self, size_t ind){
	printd("beginning channel ");printd(ind); printdln(":");
	channel_buildInterfaces(self);
	self->error_id = channel_setParam(self, ind);
	self->control = channel_INIT;
	printd("\tid: ");printdln(self->id);
	printd("\n");
}


void channel_free(Channel *self){
	;
}

int channel_start(Channel *self){
	if(self->control == channel_OFF){
		printd("starting channel ");printd(self->ind);printdln(":");
		self->control = channel_INIT;
		return 1;
	}
	return 0;
}

int channel_stop(Channel *self){
	if(self->control == channel_FAILURE){
		return 0;
	}
	printd("stopping channel ");printdln(self->ind);
	digitalWrite(self->pin, self->other_pin_state);
	self->error_id = ERROR_NO;
	self->control = channel_OFF;
	return 1;
}

int channel_disconnect(Channel *self){
	printd("disconnecting channel ");printdln(self->ind);
	if(self->control != channel_FAILURE){
		digitalWrite(self->pin, self->other_pin_state);
	}
	self->error_id = ERROR_NO;
	self->control = channel_OFF;
	return 1;
}

int channel_reset(Channel *self){
	printd("restarting channel ");printd(self->ind); printdln(":");
	if(self->control != channel_FAILURE){
		digitalWrite(self->pin, self->other_pin_state);
	}
	channel_free(self);
	channel_begin(self, self->ind);
	return 1;
}

static void channel_OFF(Channel *self){
	;
}

static void channel_FAILURE(Channel *self){
	;
}

static void channel_RUN(Channel *self) {
	CONTROL(&self->dtimer);
	int s = dtimer_getState(&self->dtimer);
	if(s == STATE_FAILURE){
		digitalWrite(self->pin, self->initial_pin_state);
		self->output = 0.0;
		self->error_id = ERROR_SUBBLOCK;
		self->control = channel_FAILURE;
		return;
	}
	if(s != self->last_dtimer_state){
		ptimer_reset(&self->ptimer);
	}
	if(dtimer_getOutput(&self->dtimer) == ON){
		CONTROL(&self->ptimer);
	}
	self->last_dtimer_state = s;
}

static void channel_INIT(Channel *self){
	if(self->error_id != ERROR_NO){
		self->control = channel_FAILURE;
		return;
	}
	if(self->initial_pin_state == HIGH){
		self->other_pin_state = LOW;
	} else {
		self->other_pin_state = HIGH;
	}
	pinMode(self->pin, OUTPUT);
	digitalWrite(self->pin, self->initial_pin_state);
	self->output = 0.0;
	dtimer_begin(&self->dtimer);
	ptimer_begin(&self->ptimer);
	if(self->enable == YES){
		self->control = channel_RUN;
	}else{
		self->control = channel_OFF;
	}
}

double channel_getOutput(Channel *self){
	return (double) self->output;
}

#ifdef USE_AOIDS
#include "aoid.c"

void channel_buildAoids(Channel *self, Aoid *next_oid, Aoid *parent_oid, size_t *id){
	channelAoid_build(&self->aoid, next_oid, parent_oid, self, id);
}

#endif

#ifdef USE_NOIDS
#include "noid.c"
#endif
