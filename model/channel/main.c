#include "main.h"

extern AppSerial serials[SERIAL_COUNT];
extern RTC rtc;

Channel channel_buf[CHANNEL_COUNT];
extern const ChannelParam CHANNEL_DEFAULT_PARAMS[];

#define TURN_EM_ON(PIN) digitalWrite((PIN), EM_ON); printdln("ON");
#define TURN_EM_OFF(PIN) digitalWrite((PIN), EM_OFF); printdln("OFF");

void channel_INIT(Channel *item);
void channel_RUN(Channel *item);
void channel_OFF(Channel *item);
void channel_FAILURE(Channel *item);

int channel_check(Channel *item){
	if(!common_checkBlockStatus(item->enable)){
		printd("channel ");printd(item->id);printd(": ");printdln("bad enable");
		return ERROR_BLOCK_STATUS;
	}
	if(item->device_kind != DEVICE_KIND_TIMER){
		printd("channel ");printd(item->id);printd(": ");printdln("bad device_kind");
		return ERROR_DEVICE_KIND;
	}
	if(dtimer_check(&item->dtimer) != ERROR_NO){
		printd("channel ");printd(item->id);printd(": ");printdln("bad dtimer");
		return ERROR_SUBBLOCK;
	}
	if(ptimer_check(&item->ptimer) != ERROR_NO){
		printd("channel ");printd(item->id);printd(": ");printdln("bad ptimer");
		return ERROR_SUBBLOCK;
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

void channel_setOutput(void *data, int state){
	Channel *channel = (Channel *) data;
	printd("set output for channel ");printd(channel->id);printd(" pin ");printdln(channel->pin);
	if(state == ON){
		TURN_EM_ON(channel->pin); return;
	}
	TURN_EM_OFF(channel->pin);
}

void channel_setDefaults(Channel *item, size_t ind){
	const ChannelParam *param = &CHANNEL_DEFAULT_PARAMS[ind];
	dtimer_setParam(&item->dtimer, (unsigned long)param->hour1*SECONDS_IN_HOUR + (unsigned long)param->minute1*SECONDS_IN_MINUTE + (unsigned long)param->second1, (unsigned long)param->hour2*SECONDS_IN_HOUR + (unsigned long)param->minute2*SECONDS_IN_MINUTE + (unsigned long)param->second2);
	ptimer_setParam(&item->ptimer,(void *) item, &channel_setOutput, param->interval_on_s, param->interval_off_s);
	item->id = param->id;
	item->enable = param->enable;
	item->pin = param->pin;
}

static void channel_setFromNVRAM(Channel *item, size_t ind){
	if(!pmem_getChannel(item, ind)){
		printdln("   failed to get channel");
		item->error_id = ERROR_PMEM_READ;
		return;
	}
}

void channel_setParam(Channel *item, size_t ind, int default_btn){
	if(default_btn == BUTTON_DOWN){
		channel_setDefaults(item, ind);
		pmem_saveChannel(item, ind);
		printd("\tdefault param\n");
	}else{
		channel_setFromNVRAM(item, ind);
		printd("\tNVRAM param\n");
	}
	item->ind = ind;
	item->device_kind = DEVICE_KIND_TIMER;
}

void channel_begin(Channel *item, size_t ind, int default_btn){
	printd("beginning channel ");printd(ind); printdln(":");
	item->error_id = ERROR_NO;
	channel_setParam(item, ind, default_btn);
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

int channel_start(Channel *item){
	printd("starting channel ");printd(item->ind);printdln(":");
	item->enable = YES;
	item->control = channel_INIT;
	CHANNEL_SAVE_FIELD(enable)
	return 1;
}

int channel_stop(Channel *item){
	printd("stopping channel ");printdln(item->ind); 
	item->enable = NO;
	item->control = channel_OFF;
	CHANNEL_SAVE_FIELD(enable)
	return 1;
}

int channel_reload(Channel *item){
	printd("reloading channel ");printd(item->ind); printdln(":");
	channel_setFromNVRAM(item, item->ind);
	item->control = channel_INIT;
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

void channels_stop(ChannelLList *channels){
	FOREACH_CHANNEL(channels){
		channel_stop(channel);
	}
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
	item->error_id = channel_check(item);
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

//void channel_control(Channel *item) {
	//switch(item->state){
		//case RUN:{
			//int s = dtimer_control(&item->dtimer, &rtc);
			//if(s == FAILURE){
				//TURN_EM_OFF(item->pin);
				//item->state = channel_FAILURE;
				//break;
			//}
			//if(s != item->last_dtimer_state){
				//ptimer_reset(&item->ptimer);
			//}
			//if(dtimer_getOutput(&item->dtimer) == ON){
				//ptimer_control(&item->ptimer);
			//}
			//item->last_dtimer_state = s;}
			//break;
		//case OFF:
			//break;
		//case FAILURE:
			//break;
		//case INIT:
			//if(item->error_id != ERROR_NO){
		        //item->state = channel_FAILURE;
		        //break;
		    //}
			//item->error_id = channel_check(item);
		    //if(item->error_id != ERROR_NO){
		        //item->state = channel_FAILURE;
		        //break;
		    //}
		    //pinMode(item->pin, OUTPUT);
			//dtimer_begin(&item->dtimer);
			//ptimer_begin(&item->ptimer);
			//item->state = channel_OFF;
			//if(item->enable == YES){
				//item->state = channel_RUN;
			//}
			//printd("\tINIT: ");printd(item->id); printd(" "); printd(" state=");printd(channel_getStateStr(item));printd(" error=");printd(channel_getErrorStr(item));printd("\n");
			//break;
		//default:
			//break;
	//}
//}

Channel * channel_getById(int id, Channel *channels, size_t channel_count){
	for(size_t i = 0; i< channel_count;i++){
		Channel *channel = &channels[i];
		if(channel->id == id){
			return channel;
		}
	}
	return NULL;
}



//const char *channel_getStatusStr(Channel *item){
	//switch(item->dptimer.state){
		//case WAIT_ON: return "OFF";
		//case WAIT_OFF: return "ON";
		//case OFF: return "dis";
		//case INIT: return "ini";
	//}
	//return "?";
//}

int CHANNEL_FUN_GET(enable)(Channel *item){return item->enable;}
int CHANNEL_FUN_GET(device_kind)(Channel *item){return item->device_kind;}
int CHANNEL_FUN_GET(pin)(Channel *item){return item->pin;}
unsigned long CHANNEL_FUN_GET(time_on)(Channel *item){return item->dtimer.time_on;}
unsigned long CHANNEL_FUN_GET(time_off)(Channel *item){return item->dtimer.time_off;}
unsigned long CHANNEL_FUN_GET(interval_on)(Channel *item){return item->ptimer.interval_on;}
unsigned long CHANNEL_FUN_GET(interval_off)(Channel *item){return item->ptimer.interval_off;}

