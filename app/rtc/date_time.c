#include "date_time.h"

unsigned long dt_dtToIDate(const DateTime *dt){
	return dt->y * DAYS_IN_YEAR + dt->m * DAYS_IN_MONTH + dt->d;
}

unsigned long dt_dtToITime(const DateTime *dt){
	return dt->hh * SECONDS_IN_HOUR + dt->mm * SECONDS_IN_MINUTE + dt->ss;
}
