#include "param.h"

err_t dtimerParam_check(const DTimerParam *self){
	if(self->time_on_s > SECONDS_IN_DAY){
		printdln("time_on is over");
		return ERROR_PARAM;
	}
	if(self->time_off_s > SECONDS_IN_DAY){
		printdln("time_off is over");
		return ERROR_PARAM;
	}
	if(self->time_on_s == self->time_off_s){
		printdln("time_on == time_off");
		return ERROR_PARAM;
	}
	return ERROR_NO;
}
