#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "CONFIG.h"

#define RGB(value) (value >> 16) & 0xFF, (value >> 8) & 0xFF, (value & 0xFF)

#define HIGH 1
#define LOW 0

#define GATE_NOT 0
#define GATE_OR 1
#define GATE_AND 2
#define GATE_XOR 3

#define TYPE_INPUT 0
#define TYPE_GATE 1
#define TYPE_CONSTANT 2
#define TYPE_OUTPUT 3

typedef struct {
	unsigned char type : 2;
} generic_t;

typedef struct {
	unsigned char type : 2;
	bool signal : 1;
	unsigned char gate : 2;
	void * left;
	void * right;
} gate_t;

typedef struct {
	unsigned char type : 2;
	bool signal : 1;
} constant_t;

typedef struct {
	unsigned char type : 2;
	bool signal : 1;
	void * left;
	void * right;
} input_t;

typedef struct {
	unsigned char type : 2;
	bool signal : 1;
	void * left;
	void * right;
} output_t;

typedef struct {
	unsigned char type : 2;
	size_t num_inputs;
	input_t ** inputs;
	size_t num_outputs;
	output_t ** outputs;
} circuit_t;

bool evaluate(generic_t * node);
bool evaluate(generic_t * node) {
	generic_t * left;
	generic_t * right;
	bool lvalue;
	bool rvalue;

	switch (node->type) {
		case TYPE_INPUT:
			left = (generic_t *) ((input_t *) node)->left;
			if (left == NULL) {
				lvalue = LOW;
			} else {
				lvalue = evaluate(left);
			}

			return lvalue;
		case TYPE_CONSTANT:
			return ((constant_t *) node)->signal;
		case TYPE_GATE:
			left = (generic_t *) ((gate_t *) node)->left;
			right = (generic_t *) ((gate_t *) node)->right;
			if (left == NULL) {
				lvalue = LOW;
			} else {
				lvalue = evaluate(left);
			}

			if (right == NULL) {
				rvalue = LOW;
			} else {
				rvalue = evaluate(right);
			}

			switch (((gate_t *) node)->gate) {
				case GATE_NOT:
					return (lvalue == HIGH) ? LOW : HIGH;
				case GATE_AND:
					return (lvalue == HIGH && rvalue == HIGH) ? HIGH : LOW;
				case GATE_OR:
					return (lvalue == HIGH || rvalue == HIGH) ? HIGH : LOW;
				case GATE_XOR:
					return ((lvalue == HIGH && rvalue == LOW) || (rvalue == HIGH && lvalue == LOW)) ? HIGH : LOW;
				default:
					fprintf(stderr, "evaluate(): invalid gate type %u\n", ((gate_t *) node)->gate);
					abort();
			}
		case TYPE_OUTPUT:
			left = (generic_t *) ((gate_t *) node)->left;
			right = (generic_t *) ((gate_t *) node)->right;
			if (left == NULL) {
				lvalue = LOW;
			} else {
				lvalue = evaluate(left);
			}

			return lvalue;
		default: return LOW;
	}
}

bool * evaluate_circuit(circuit_t * circuit) {
	size_t i;
	bool * outputs = malloc(circuit->num_outputs * sizeof(bool));
	if (outputs == NULL) {
		fprintf(stderr, "error: evaluate_circuit(): malloc(circuit->num_outputs * sizeof(bool)) failed\n");
		abort();
	}

	for (i = 0; i < circuit->num_outputs; ++i) {
		outputs[i] = evaluate((generic_t *) circuit->outputs[i]);
	}

	return outputs;
}

int main() {
	return 0;
}
