#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

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
} circuit_t;

bool evaluate(generic_t * node);
bool evaluate(generic_t * node) {
	generic_t * left;
	generic_t * right;
	bool lvalue;
	bool rvalue;

	switch (node->type) {
		case TYPE_CIRCUIT:
			return LOW;
		case TYPE_INPUT:
			return ((input_t *) node)->signal;
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

int main(int argc, char ** argv) {
	argc = argc;
	circuit_t circuit;
	gate_t xgate, ogate, xgate2, agate, agate2;
	input_t a, b, c;
	
	circuit.num_outputs = 2;
	circuit.outputs = malloc(sizeof(output_t *) * circuit.num_outputs);
	circuit.outputs[0] = malloc(sizeof(output_t));
	circuit.outputs[1] = malloc(sizeof(output_t));

	circuit.outputs[0]->type = TYPE_OUTPUT;
	circuit.outputs[0]->left = &xgate;

	circuit.outputs[1]->type = TYPE_OUTPUT;
	circuit.outputs[1]->left = &ogate;
	
	xgate.type = TYPE_GATE;
	xgate.gate = GATE_XOR;
	xgate.left = &xgate2;
	xgate.right = &c;

	xgate2.type = TYPE_GATE;
	xgate2.gate = GATE_XOR;
	xgate2.left = &a;
	xgate2.right = &b;

	ogate.type = TYPE_GATE;
	ogate.gate = GATE_OR;
	ogate.left = &agate;
	ogate.right = &agate2;

	agate.type = TYPE_GATE;
	agate.gate = GATE_AND;
	agate.left = &xgate2;
	agate.right = &c;

	agate2.type = TYPE_GATE;
	agate2.gate = GATE_AND;
	agate2.left = &a;
	agate2.right = &b;

	a.type = TYPE_INPUT;
	a.signal = atoi(argv[1]);
	b.type = TYPE_INPUT;
	b.signal = atoi(argv[2]);
	c.type = TYPE_INPUT;
	c.signal = atoi(argv[3]);

	bool * out = evaluate_circuit(&circuit);

	for (size_t i = 0; i < circuit.num_outputs; ++i) {
		printf("%llu: %u\n", (long long unsigned int) i, out[i]);
	}

	free(out);

	return 0;
}
