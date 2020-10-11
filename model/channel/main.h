#ifndef MODEL_CHANNEL_H
#define MODEL_CHANNEL_H

#include "../../util/dstructure.h"
#include "../../util/common.h"
#include "../../pmem/main.h"
#include "../../app/serial.h"
#include "../dtimer.h"
#include "../ptimer.h"
#include "param.h"

#define EM_ON	LOW
#define EM_OFF	HIGH

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
	void (*control)(struct channel_st *);
	
	struct channel_st *next;
};
typedef struct channel_st Channel;

DEC_LLIST(Channel)

#define FOREACH_CHANNEL(LIST) FOREACH_LLIST(channel, LIST, Channel)
#define CHANNEL_SAVE_FIELD(F) ChannelParam pchannel;	if(pmem_getPChannel(&pchannel, item->ind)){pchannel.F = item->F; pmem_savePChannel(&pchannel, item->ind);}
#define CHANNEL_FUN_GET(param) channel_get_ ## param

extern void channel_begin(Channel *item, size_t ind, int default_btn);
extern void channels_buildFromArray(ChannelLList *channels, Channel arr[]);
extern void channels_begin(ChannelLList *channels, int default_btn);
extern void channel_free(Channel *item);
extern int channel_start(Channel *item);
extern int channel_stop(Channel *item);
extern int channel_disconnect(Channel *item);
extern int channel_reset(Channel *item);
extern int channels_activeExists(ChannelLList *channels);
extern double channel_getOutput(Channel *item);
extern int channels_getIdFirst(ChannelLList *channels, int *out);
extern const char *channel_getErrorStr(Channel *item);
extern const char *channel_getStateStr(Channel *item);
extern int CHANNEL_FUN_GET(enable)(Channel *item);
extern int CHANNEL_FUN_GET(device_kind)(Channel *item);
extern int CHANNEL_FUN_GET(pin)(Channel *item);
extern unsigned long CHANNEL_FUN_GET(time_on_s)(Channel *item);
extern unsigned long CHANNEL_FUN_GET(time_off_s)(Channel *item);
extern unsigned long CHANNEL_FUN_GET(interval_on_s)(Channel *item);
extern unsigned long CHANNEL_FUN_GET(interval_off_s)(Channel *item);

#endif 
