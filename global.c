#include <global.h>
#include <stdbool.h>

static GLOBAL_t global;

void global_close() {
	global.close = 1;
}

bool global_should_close() {
	return global.close;
}
