#include "param.h"
#include "../../util/common.h"
/*
 * -user_config:
 * put parameters for each channel in this array
 * enable: YES or NO
 * {id, enable, pin, hour1,minute1,second1, hour2,minute2,second2, interval_on_s,interval_off_s}
 */
 
#define SET_CPARAM_ELEMENT(id, enable, pin, hour1,minute1,second1, hour2,minute2,second2, interval_on_s,interval_off_s) {id, enable, pin, (unsigned long)hour1*SECONDS_IN_HOUR + (unsigned long)minute1*SECONDS_IN_MINUTE + (unsigned long)second1, (unsigned long)hour2*SECONDS_IN_HOUR + (unsigned long)minute2*SECONDS_IN_MINUTE + (unsigned long)second2, interval_on_s,interval_off_s}
const ChannelParam CHANNEL_DEFAULT_PARAMS[] = {
	SET_CPARAM_ELEMENT(21, YES, 3, 11,11,0, 11,12,0, 3,4),
	SET_CPARAM_ELEMENT(22, YES, 3, 11,11,0, 11,12,0, 3,4),
	SET_CPARAM_ELEMENT(23, YES, 3, 11,13,0, 11,14,0, 3,4)
};

#define CHANNEL_COUNT (sizeof CHANNEL_DEFAULT_PARAMS / sizeof CHANNEL_DEFAULT_PARAMS[0])
