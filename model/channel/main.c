#include "main.h"

extern AppSerial serials[SERIAL_COUNT];
extern RTC rtc;

Channel channel_buf[CHANNEL_COUNT];
extern const ChannelParam CHANNEL_DEFAULT_PARAMS[];

#define TURN_EM_ON(PIN) digitalWrite((PIN), EM_ON); printdln("ON");
#define TURN_EM_OFF(PIN) digitalWrite((PIN), EM_OFF); printdln("OFF");

static void channel_INIT(Channel *item);
static void channel_RUN(Channel *item);
static void channel_OFF(Channel *item);
static void channel_FAILURE(Channel *item);

static int channelParam_check(const ChannelParam *item){
	if(!common_checkBlockStatus(item->enable)){
		printd("channel ");printd(item->id);printd(": ");printdln("bad enable");
		return ERROR_BLOCK_STATUS;
	}
	int r = dtimer_checkParam(item->time_on_s, item->time_off_s);
	if(r != ERROR_NO){
		printd("channel ");printd(item->id);printd(": ");printdln("bad dtimer");
		return r;
	}
	r = ptimer_checkParam(item->interval_on_s, item->interval_off_s);
	if(r != ERROR_NO){
		printd("channel ");printd(item->id);printd(": ");printdln("bad ptimer");
		return r;
	}
    return ERROR_NO;
}

const char *channel_getErrorStr(Channel *item){
	return getErrorStr(item->error_id);
}

const char *channel_getStateStr(Channel *item){
	if(item->control == channel_RUN)			return "RUN";
	else if(item->control == channel_OFF)		return "OFF";
	else if(item->control == channel_INIT)		return "INIT";
	else if(item->control == channel_FAILURE)	return "FAILURE";
	return "?";
}

static void channel_setOutput(void *data, int state){
	Channel *channel = (Channel *) data;
	printd("set output for channel ");printd(channel->id);printd(" pin ");printdln(channel->pin);
	if(state == ON){
		TURN_EM_ON(channel->pin); return;
	}
	TURN_EM_OFF(channel->pin);
}

static int channel_setParam(Channel *item, const ChannelParam *param){
	int r = channelParam_check(param);
	if(r != ERROR_NO){
		return r;
	}
	dtimer_setParam(&item->dtimer, param->time_on_s, param->time_off_s);
	ptimer_setParam(&item->ptimer,(void *) item, &channel_setOutput, param->interval_on_s, param->interval_off_s);
	item->id = param->id;
	item->device_kind = DEVICE_KIND_TIMER;
	item->enable = param->enable;
	item->pin = param->pin;
	return ERROR_NO;
}

static int channel_setDefaults(Channel *item, size_t ind){
	const ChannelParam *param = &CHANNEL_DEFAULT_PARAMS[ind];
	int r = channel_setParam(item, param);
	if(r == ERROR_NO){
		pmem_savePChannel(param, ind);
	}
	return r;
}

static int channel_setFromNVRAM(Channel *item, size_t ind){
	ChannelParam param;
	if(!pmem_getPChannel(&param, ind)){
		printdln("   failed to get channel from NVRAM");
		return ERROR_PMEM_READ;
	}
	return channel_setParam(item, &param);
}

static int channel_setParamAlt(Channel *item, size_t ind, int default_btn){
	if(default_btn == BUTTON_DOWN){
		int r = channel_setDefaults(item, ind);
		if(r != ERROR_NO){
			return r;
		}
		printd("\tdefault param\n");
	}else{
		int r = channel_setFromNVRAM(item, ind);
		if(r != ERROR_NO){
			return r;
		}
		printd("\tNVRAM param\n");
	}
	item->ind = ind;
	return ERROR_NO;
}

void channel_begin(Channel *item, size_t ind, int default_btn){
	printd("beginning channel ");printd(ind); printdln(":");
	item->error_id = channel_setParamAlt(item, ind, default_btn);
	item->control = channel_INIT;
	printd("\tid: ");printdln(item->id);
	printd("\n");
}

void channels_buildFromArray(ChannelLList *channels, Channel arr[]){
	if(CHANNEL_COUNT <= 0) return;
	channels->length = CHANNEL_COUNT;
	channels->top = &arr[0];
	channels->last = &arr[CHANNEL_COUNT - 1];
	for(size_t i = 0;i<CHANNEL_COUNT;i++){
		arr[i].next = NULL;
	}
	for(size_t i = 0; i<CHANNEL_COUNT-1; i++){
		arr[i].next = &arr[i+1];
	}
}

void channels_begin(ChannelLList *channels, int default_btn){
	extern Channel channel_buf[CHANNEL_COUNT];
	channels_buildFromArray(channels, channel_buf);
	size_t i = 0;
	FOREACH_CHANNEL(channels){
		channel_begin(channel, i, default_btn); i++;
	}
}

void channel_free(Channel *item){
	;
}

int channel_start(Channel *item){
	if(item->control == channel_OFF || item->control == channel_FAILURE){
		printd("starting channel ");printd(item->ind);printdln(":");
		item->enable = YES;
		item->control = channel_INIT;
		CHANNEL_SAVE_FIELD(enable)
		return 1;
	}
	return 0;
}

int channel_stop(Channel *item){
	printd("stopping channel ");printdln(item->ind); 
	item->enable = NO;
	item->control = channel_OFF;
	CHANNEL_SAVE_FIELD(enable)
	return 1;
}

int channel_disconnect(Channel *item){
	printd("disconnecting channel ");printdln(item->ind);
	item->error_id = ERROR_NO;
	item->control = channel_OFF;
	return 1;
}

int channel_reset(Channel *item){
	printd("resetting channel ");printd(item->ind); printdln(":");
	channel_free(item);
	channel_begin(item, item->ind, digitalRead(DEFAULT_CONTROL_PIN));
	return 1;
}

int channels_activeExists(ChannelLList *channels){
	FOREACH_CHANNEL(channels){
		if(channel->control != channel_OFF){
			return 1;
		}
	}
	return 0;
}

double channel_getOutput(Channel *item){
	int dt_out = dtimer_getOutput(&item->dtimer);
	int pt_out = ptimer_getOutput(&item->ptimer);
	if(dt_out == ON && pt_out == ON){
		return 1.0;
	}
	return 0.0;
}

void channel_INIT(Channel *item) {
	if(item->error_id != ERROR_NO){
		item->control = channel_FAILURE;
		return;
	}
	pinMode(item->pin, OUTPUT);
	dtimer_begin(&item->dtimer);
	ptimer_begin(&item->ptimer);
	item->control = channel_OFF;
	if(item->enable == YES){
		item->control = channel_RUN;
	}
	printd("\tINIT: ");printd(item->id); printd(" "); printd(" state=");printd(channel_getStateStr(item));printd(" error=");printd(channel_getErrorStr(item));printd("\n");
}

void channel_RUN(Channel *item) {
	CONTROL(&item->dtimer);
	int s = dtimer_getState(&item->dtimer);
	if(s == FAILURE){
		TURN_EM_OFF(item->pin);
		item->control = channel_FAILURE;
		return;
	}
	if(s != item->last_dtimer_state){
		ptimer_reset(&item->ptimer);
	}
	if(dtimer_getOutput(&item->dtimer) == ON){
		CONTROL(&item->ptimer);
	}
	item->last_dtimer_state = s;
}

void channel_OFF(Channel *item) {
	;
}

void channel_FAILURE(Channel *item) {
	;
}

int channels_getIdFirst(ChannelLList *channels, int *out){
	int success = 0;
	int f = 0;
	int v;
	FOREACH_CHANNEL(channels){
		if(!f) { v=channel->id; f=1; success=1;}
		if(channel->id < v) v = channel->id;
	}
	*out = v;
	return success;
}

int CHANNEL_FUN_GET(enable)(Channel *item){return item->enable;}
int CHANNEL_FUN_GET(device_kind)(Channel *item){return item->device_kind;}
int CHANNEL_FUN_GET(pin)(Channel *item){return item->pin;}
unsigned long CHANNEL_FUN_GET(time_on_s)(Channel *item){return item->dtimer.time_on;}
unsigned long CHANNEL_FUN_GET(time_off_s)(Channel *item){return item->dtimer.time_off;}
unsigned long CHANNEL_FUN_GET(interval_on_s)(Channel *item){return item->ptimer.interval_on;}
unsigned long CHANNEL_FUN_GET(interval_off_s)(Channel *item){return item->ptimer.interval_off;}
