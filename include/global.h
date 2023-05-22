#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdbool.h>

typedef struct GLOBAL {
	bool close : 1;
} GLOBAL_t;

void global_close();
bool global_should_close();

#endif
