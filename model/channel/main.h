#ifndef MODEL_CHANNEL_H
#define MODEL_CHANNEL_H


#include "../../util/dstructure.h"
#include "../../util/common.h"
#include "../../pmem/main.h"
#include "../../app/serial.h"
#include "../dtimer.h"
#include "../ptimer.h"
#include "param.h"

#define EM_ON LOW
#define EM_OFF HIGH

struct channel_st {
	int id;
	int device_kind;
	size_t ind;//index in array
	int enable;
	
	int pin;
	DTimer dtimer;
	PTimer ptimer;
	int last_dtimer_state;
	
	int error_id;
	int state;
	
	struct channel_st *next;
};
typedef struct channel_st Channel;

DEC_LLIST(Channel)

#define FOREACH_CHANNEL(LIST) FOREACH_LLIST(channel, LIST, Channel){
#define CHANNEL_SAVE_FIELD(F) PmemChannel pchannel;	if(pmem_getPChannel(&pchannel, item->ind)){pchannel.F = item->F; pmem_savePChannel(&pchannel, item->ind);}
#define CHANNEL_FUN_GET(param) channel_get_ ## param

#endif 
