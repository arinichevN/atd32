#include "param.h"
#include "../../util/common.h"
/*
 * -user_config:
 * put parameters for each channel in this array
 * enable: YES or NO
 * {id, enable, pin, hour1,minute1,second1, hour2,minute2,second2, interval_on_s,interval_off_s}
 */
const ChannelParam CHANNEL_DEFAULT_PARAMS[] = {
	{21, YES, 3, 11,11,0, 11,12,0, 3,4},
	{22, YES, 3, 11,11,0, 11,12,0, 3,4},
	{23, YES, 3, 11,13,0, 11,14,0, 3,4}
};

#define CHANNEL_COUNT (sizeof CHANNEL_DEFAULT_PARAMS / sizeof CHANNEL_DEFAULT_PARAMS[0])
