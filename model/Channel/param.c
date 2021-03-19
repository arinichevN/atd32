#include "param.h"

err_t channelParam_check(const ChannelParam *self){
	switch(self->initial_pin_state){
		case HIGH: case LOW:
			break;
		default:
			return ERROR_PARAM;
	}
	switch(self->enable){
		case YES: case NO:
			break;
		default:
			return ERROR_PARAM;
	}
	if(dtimerParam_check(&self->dtimer) != ERROR_NO){
		return ERROR_PARAM;
	}
	if(ptimerParam_check(&self->ptimer) != ERROR_NO){
		return ERROR_PARAM;
	}
	return ERROR_NO;
}
