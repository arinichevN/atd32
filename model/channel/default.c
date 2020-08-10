#include "param.h"

/*
 * put parameters for each channel in this array
 * {ID, ENABLE, PIN, HOUR1,MINUTE1,SECOND1, HOUR2,MINUTE2,SECOND2, INTERVAL_ON_S,INTERVAL_OFF_S}
 */
const ChannelParam CHANNEL_DEFAULT_PARAMS[] = {
	{21, 1, 3, 11,11,0, 11,12,0, 3,4},
	{22, 1, 3, 11,11,0, 11,12,0, 3,4},
	{23, 1, 3, 11,13,0, 11,14,0, 3,4}
};

#define CHANNEL_COUNT (sizeof CHANNEL_DEFAULT_PARAMS / sizeof CHANNEL_DEFAULT_PARAMS[0])
