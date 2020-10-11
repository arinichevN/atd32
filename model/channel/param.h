#ifndef MODEL_CHANNEL_PARAM_H
#define MODEL_CHANNEL_PARAM_H


typedef struct {
	int id;
	int enable;
	int pin;
	unsigned long time_on_s;
	unsigned long time_off_s;
	unsigned long interval_on_s;
	unsigned long interval_off_s;
} ChannelParam;

#define CHANNEL_DEFAULT_PARAMS chdefps
#endif 
