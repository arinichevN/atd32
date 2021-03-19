#ifndef MODEL_ISLAVE_H
#define MODEL_ISLAVE_H

#include "../../lib/common.h"

typedef struct {
	void *self;
	void (*perform)(void *, int);
} iSlave;

#endif 
