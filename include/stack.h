#ifndef STACK_H
#define STACK_H

#include <stdint.h>
#include <stddef.h>
#include <type.h>

typedef struct {
	size_t size;
	size_t max;
	type_t type;
	void * array;
} stack_t;

stack_t * stack_new(type_t type, size_t size);
void stack_free(stack_t * stack);
void stack_push(stack_t * stack, value_t value);
value_t stack_pop(stack_t * stack);
value_t stack_peek(stack_t * stack);
void stack_clear(stack_t * stack);
void stack_print(stack_t * stack);

#endif
