#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <type.h>
#include <stack.h>

#define HIGH 1
#define LOW 0

#define GATE_NOT 0
#define GATE_OR 1
#define GATE_AND 2
#define GATE_XOR 3

#define TYPE_CIRCUIT 0
#define TYPE_GATE 1
#define TYPE_INPUT 2
#define TYPE_OUTPUT 3

typedef unsigned char bool;

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
} input_t;

typedef struct {
	unsigned char type : 2;
	bool signal : 1;
	void * left;
	void * right;
} output_t;

typedef struct {
	unsigned char type : 2;
	size_t num_outputs;
	output_t ** outputs;
	size_t num_inputs;
	input_t ** inputs;
} circuit_t;

bool evaluate(generic_t * node);
bool evaluate(generic_t * node) {
	generic_t * left;
	generic_t * right;
	bool lvalue;
	bool rvalue;

	switch (node->type) {
		case TYPE_CIRCUIT:
			return 0;
		case TYPE_INPUT:
			return ((input_t *) node)->signal;
		case TYPE_GATE:
			left = (generic_t *) ((gate_t *) node)->left;
			right = (generic_t *) ((gate_t *) node)->right;
			if (left == NULL) {
				printf("left == NULL\n");
				lvalue = 0;
			} else {
				printf("left != NULL\n");
				lvalue = evaluate(left);
			}

			if (right == NULL) {
				printf("right == NULL\n");
				rvalue = 0;
			} else {
				printf("right != NULL\n");
				rvalue = evaluate(right);
			}

			switch (((gate_t *) node)->gate) {
				case GATE_NOT:
					return (lvalue == 1) ? 0 : 1;
				case GATE_AND:
					return (lvalue == 1 && rvalue == 1) ? 1 : 0;
				case GATE_OR:
					return (lvalue == 1 || rvalue == 1) ? 1 : 0;
				case GATE_XOR:
					return ((lvalue == 1 && rvalue == 0) || (rvalue == 1 && lvalue == 0)) ? 1 : 0;
				default:
					fprintf(stderr, "evaluate(): invalid gate type %u\n", ((gate_t *) node)->gate);
					abort();
			}
		case TYPE_OUTPUT:
			left = (generic_t *) ((gate_t *) node)->left;
			right = (generic_t *) ((gate_t *) node)->right;
			if (left == NULL) {
				lvalue = 0;
			} else {
				lvalue = evaluate(left);
			}

			return lvalue;
		default: return 0;
	}
}

int main(void) {
	circuit_t circuit;
	gate_t * ngate = malloc(sizeof(gate_t));
	if (ngate == NULL) { printf("ngate fail\n"); return -1; }
	input_t * a = malloc(sizeof(input_t));
	if (a == NULL) { printf("input fail\n"); return -1; }
	input_t * b = malloc(sizeof(input_t));
	if (b == NULL) { printf("input fail\n"); return -1; }

	circuit.num_outputs = 1;
	circuit.outputs = malloc(sizeof(output_t *) * circuit.num_outputs);
	circuit.outputs[0] = malloc(sizeof(output_t));
	circuit.num_inputs = 1;
	circuit.inputs = malloc(sizeof(input_t) * circuit.num_inputs);

	circuit.outputs[0]->type = TYPE_OUTPUT;
	circuit.outputs[0]->left = (void *) ngate;
	ngate->type = TYPE_GATE;
	ngate->gate = GATE_XOR;
	ngate->left = a;
	ngate->right = b;
	a->type = TYPE_INPUT;
	a->signal = 0;
	b->type = TYPE_INPUT;
	b->signal = 1;

	printf("%u\n", evaluate((generic_t *) circuit.outputs[0]));

	return 0;
}
