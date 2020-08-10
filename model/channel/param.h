#ifndef MODEL_CHANNEL_PARAM_H
#define MODEL_CHANNEL_PARAM_H


typedef struct {
	int id;
	int enable;
	int pin;
	uint8_t hour1;
	uint8_t minute1;
	uint8_t second1;
	uint8_t hour2;
	uint8_t minute2;
	uint8_t second2;
	unsigned long interval_on_s;
	unsigned long interval_off_s;
} ChannelParam;

#define CHANNEL_DEFAULT_PARAMS chdefps
#endif 
