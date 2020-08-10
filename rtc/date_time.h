#ifndef DATETIME_H
#define DATETIME_H


#define MILLISECONDS_IN_SECOND	1000UL
#define SECONDS_IN_MINUTE		60UL
#define SECONDS_IN_HOUR			3600UL
#define SECONDS_IN_DAY			86400UL 
#define DAYS_IN_YEAR			403UL
#define DAYS_IN_MONTH			31UL

typedef struct {
	uint16_t y;
	uint8_t m;
	uint8_t d;
	uint8_t hh;
	uint8_t mm;
	uint8_t ss;
} DateTime;

extern unsigned long dt_dtToIDate(const DateTime *dt);

extern unsigned long dt_dtToITime(const DateTime *dt);

#endif 
