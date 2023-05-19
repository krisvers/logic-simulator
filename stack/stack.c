#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <type.h>
#include <stack.h>

/*
	stack_t * stack_new(type_t type, size_t max);

NOTE:
	Returns a pointer malloced container. Requires a type (type_t enum), size. How to free: end-programmer needs to call stack_free(stack).
*/
stack_t * stack_new(type_t type, size_t max) {
	stack_t * stack = malloc(sizeof(stack_t));
	if (stack == NULL) {
		fprintf(stderr, "error: malloc(sizeof(stack_t)) failed\n");
		abort();
	}

	stack->max = max;
	stack->size = 0;
	stack->type = type;
	
	if (max != 0) {
		DO_FOR_EACH_TYPE(type, \
			stack->array = NULL, \
			stack->array = malloc(sizeof(uint8_t) * max), \
			stack->array = malloc(sizeof(uint16_t) * max), \
			stack->array = malloc(sizeof(uint32_t) * max), \
			stack->array = malloc(sizeof(uint64_t) * max), \
			stack->array = malloc(sizeof(int8_t) * max), \
			stack->array = malloc(sizeof(int16_t) * max), \
			stack->array = malloc(sizeof(int32_t) * max), \
			stack->array = malloc(sizeof(int64_t) * max), \
			stack->array = malloc(sizeof(ptr_t) * max) \
		);
	}

	return stack;
}

/*
	void stack_free(stack_t * stack);

NOTE:
	Takes a stack_t pointer, frees the array, and frees the stack pointer.
*/
void stack_free(stack_t * stack) {
	if (stack == NULL) {
		fprintf(stderr, "error: cannot free NULL stack\n");
		abort();
	}

	if (stack->max == 0) {
		free(stack->array);
	}

	free((void *) stack);
}

/*
	void stack_push(stack_t * stack, value_t);

NOTE:
	Takes a stack_t pointer and generic value, if the array is dynamic, it reallocates to bigger size and pushes the value.
*/
void stack_push(stack_t * stack, value_t value) {
	if (stack->max != 0 && stack->size >= stack->max) {
		fprintf(stderr, "error: stack overflow (%s): %p\n", type_get_name(stack->type), (void *) stack);
		abort();
	}

	if (stack->max == 0) {
		DO_FOR_EACH_TYPE(stack->type, \
			break, \
			stack->array = realloc(stack->array, (size_t) (((arithptr_t) stack->size) + 1) * sizeof(uint8_t)), \
			stack->array = realloc(stack->array, (size_t) (((arithptr_t) stack->size) + 1) * sizeof(uint16_t)), \
			stack->array = realloc(stack->array, (size_t) (((arithptr_t) stack->size) + 1) * sizeof(uint32_t)), \
			stack->array = realloc(stack->array, (size_t) (((arithptr_t) stack->size) + 1) * sizeof(uint64_t)), \
			stack->array = realloc(stack->array, (size_t) (((arithptr_t) stack->size) + 1) * sizeof(int8_t)), \
			stack->array = realloc(stack->array, (size_t) (((arithptr_t) stack->size) + 1) * sizeof(int16_t)), \
			stack->array = realloc(stack->array, (size_t) (((arithptr_t) stack->size) + 1) * sizeof(int32_t)), \
			stack->array = realloc(stack->array, (size_t) (((arithptr_t) stack->size) + 1) * sizeof(int64_t)), \
			stack->array = realloc(stack->array, (size_t) (((arithptr_t) stack->size) + 1) * sizeof(ptr_t)) \
		);
	}

	DO_FOR_EACH_TYPE(stack->type, \
		break, \
		((uint8_t *) stack->array)[stack->size] = (uint8_t) value, \
		((uint16_t *) stack->array)[stack->size] = (uint16_t) value, \
		((uint32_t *) stack->array)[stack->size] = (uint32_t) value, \
		((uint64_t *) stack->array)[stack->size] = (uint64_t) value, \
		((int8_t *) stack->array)[stack->size] = (int8_t) value, \
		((int16_t *) stack->array)[stack->size] = (int16_t) value, \
		((int32_t *) stack->array)[stack->size] = (int32_t) value, \
		((int64_t *) stack->array)[stack->size] = (int64_t) value, \
		((ptr_t *) stack->array)[stack->size] = (ptr_t) (uintptr_t) value \
	);

	if (stack->size == SIZE_MAX) {
		fprintf(stderr, "error: stack overflow (%s): %p\n", type_get_name(stack->type), (void *) stack);
		abort();
	}

	++stack->size;

	return;
}

/*
	void stack_pop(stack_t * stack);

NOTE:
	Takes a stack_t pointer. Saves and frees the top element. If the array is dynamic, realloc to smaller array.
*/
value_t stack_pop(stack_t * stack) {
	value_t value;
	
	if (stack->size == 0) {
		fprintf(stderr, "error: stack underflow (%s): %p\n", type_get_name(stack->type), (void *) stack);
		abort();
	}

	--stack->size;

	DO_FOR_EACH_TYPE(stack->type, \
		value = (value_t) (arithptr_t) NULL, \
		value = (value_t) ((uint8_t *) stack->array)[stack->size], \
		value = (value_t) ((uint16_t *) stack->array)[stack->size], \
		value = (value_t) ((uint32_t *) stack->array)[stack->size], \
		value = (value_t) ((uint64_t *) stack->array)[stack->size], \
		value = (value_t) ((int8_t *) stack->array)[stack->size], \
		value = (value_t) ((int16_t *) stack->array)[stack->size], \
		value = (value_t) ((int32_t *) stack->array)[stack->size], \
		value = (value_t) ((int64_t *) stack->array)[stack->size], \
		value = (value_t) (arithptr_t) ((ptr_t *) stack->array)[stack->size] \
	);

	if (stack->max == 0) {
		DO_FOR_EACH_TYPE(stack->type, \
			break, \
			stack->array = realloc(stack->array, (size_t) ((arithptr_t) stack->size) * sizeof(uint8_t)), \
			stack->array = realloc(stack->array, (size_t) ((arithptr_t) stack->size) * sizeof(uint16_t)), \
			stack->array = realloc(stack->array, (size_t) ((arithptr_t) stack->size) * sizeof(uint32_t)), \
			stack->array = realloc(stack->array, (size_t) ((arithptr_t) stack->size) * sizeof(uint64_t)), \
			stack->array = realloc(stack->array, (size_t) ((arithptr_t) stack->size) * sizeof(int8_t)), \
			stack->array = realloc(stack->array, (size_t) ((arithptr_t) stack->size) * sizeof(int16_t)), \
			stack->array = realloc(stack->array, (size_t) ((arithptr_t) stack->size) * sizeof(int32_t)), \
			stack->array = realloc(stack->array, (size_t) ((arithptr_t) stack->size) * sizeof(int64_t)), \
			stack->array = realloc(stack->array, (size_t) ((arithptr_t) stack->size) * sizeof(ptr_t)) \
		);
	}

	return value;
}

/*
	void stack_peek(stack_t * stack);

NOTE:
	Takes a stack_t pointer and returns the top element without popping.
*/
value_t stack_peek(stack_t * stack) {
	value_t value;

	if (stack->size == 0) {
		fprintf(stderr, "error: stack cannot access top element, stack->size == 0 (%s): %p\n", type_get_name(stack->type), (void *) stack);
		abort();
	}

	DO_FOR_EACH_TYPE(stack->type, \
		value = (value_t) (arithptr_t) NULL, \
		value = (value_t) ((uint8_t *) stack->array)[stack->size - 1], \
		value = (value_t) ((uint16_t *) stack->array)[stack->size - 1], \
		value = (value_t) ((uint32_t *) stack->array)[stack->size - 1], \
		value = (value_t) ((uint64_t *) stack->array)[stack->size - 1], \
		value = (value_t) ((int8_t *) stack->array)[stack->size - 1], \
		value = (value_t) ((int16_t *) stack->array)[stack->size - 1], \
		value = (value_t) ((int32_t *) stack->array)[stack->size - 1], \
		value = (value_t) ((int64_t *) stack->array)[stack->size - 1], \
		value = (value_t) (arithptr_t) ((ptr_t *) stack->array)[stack->size - 1] \
	);

	return value;
}

/*
	void stack_clear(stack_t * stack);

NOTE:
	Takes a stack_t pointer. If the array is dynamic, then it will realloc to size 0, otherwise it will just zero out each element.
*/
void stack_clear(stack_t * stack) {
	if (stack->max == 0) {
		stack->array = realloc(stack->array, 0);
		stack->size = 0;

		return;
	}

	for (; --stack->size;) {
		DO_FOR_EACH_TYPE(stack->type, \
			;, \
			((uint8_t *) stack->array)[stack->size] = 0, \
			((uint16_t *) stack->array)[stack->size] = 0, \
			((uint32_t *) stack->array)[stack->size] = 0, \
			((uint64_t *) stack->array)[stack->size] = 0, \
			((int8_t *) stack->array)[stack->size] = 0, \
			((int16_t *) stack->array)[stack->size] = 0, \
			((int32_t *) stack->array)[stack->size] = 0, \
			((int64_t *) stack->array)[stack->size] = 0, \
			((ptr_t *) stack->array)[stack->size] = NULL \
		);
	}
}

/*
	void stack_print(stack_t * stack);

NOTE:
	Takes a stack_t pointer. Loops over each item using a pointer, incrementing by the size of the type. Then prints with the stack type, pointer and fancy [  ].
*/
void stack_print(stack_t * stack) {
	value_t i = 0;
	ptr_t ptr = stack->array;

	if (stack->size == 0 || (stack->size > stack->max && stack->max != 0)) {
		printf("stack (%s): %p: [ EMPTY ]\n", type_get_name(stack->type), (void *) stack);

		return;
	}

	printf("stack (%s): %p: [ ", type_get_name(stack->type), (void *) stack);

	for (; i < stack->size; i++) {
		if (stack->type == TYPE_PTR) {
			printf("%p", *((void **) ptr));
		} else {
			printf(type_get_printf_formatter(stack->type), type_ptr_dereference(stack->type, ptr));
		}

		printf(", ");
		ptr = type_ptr_value_add(ptr, type_sizeof(stack->type));
	}

	puts("]");
}
