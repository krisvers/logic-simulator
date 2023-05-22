#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdbool.h>

typedef struct GLOBAL {
	bool close : 1;
} GLOBAL_t;

extern GLOBAL_t global;

#endif
