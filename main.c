#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

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
	unsigned char type;
	bool signal;
} generic_t;

typedef struct {
	unsigned char type;
	bool signal;
	unsigned char gate;
	void * left;
	void * right;
} gate_t;

typedef struct {
	unsigned char type;
	bool signal;
} constant_t;

typedef struct {
	unsigned char type;
	bool signal;
	void * left;
} input_t;

typedef struct {
	unsigned char type;
	bool signal;
	void * left;
} output_t;

typedef struct {
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

			node->signal = lvalue;
			return lvalue;
		case TYPE_CONSTANT:
			node->signal = ((constant_t *) node)->signal;
			return node->signal;
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
					node->signal = (lvalue == HIGH) ? LOW : HIGH;
					return node->signal;
				case GATE_AND:
					node->signal = (lvalue == HIGH && rvalue == HIGH) ? HIGH : LOW;
					return node->signal;
				case GATE_OR:
					node->signal = (lvalue == HIGH || rvalue == HIGH) ? HIGH : LOW;
					return node->signal;
				case GATE_XOR:
					node->signal = ((lvalue == HIGH && rvalue == LOW) || (rvalue == HIGH && lvalue == LOW)) ? HIGH : LOW;
					return node->signal;
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

			node->signal = lvalue;
			return lvalue;
		default: node->signal = LOW; return LOW;
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

output_t * output_new() {
	output_t * o = malloc(sizeof(output_t));
	if (o == NULL) {
		fprintf(stderr, "error: output_new(): malloc(sizeof(output_t)) failed\n");
		abort();
	}

	o->type = TYPE_OUTPUT;
	o->signal = LOW;
	o->left = NULL;

	return o;
}

input_t * input_new() {
	input_t * i = malloc(sizeof(input_t));
	if (i == NULL) {
		fprintf(stderr, "error: input_new(): malloc(sizeof(input_t)) failed\n");
		abort();
	}

	i->type = TYPE_INPUT;
	i->left = NULL;
	i->signal = LOW;

	return i;
}

circuit_t * circuit_new(size_t inputs, size_t outputs) {
	circuit_t * c = malloc(sizeof(circuit_t));
	if (c == NULL) {
		fprintf(stderr, "error: circuit_new(): malloc(sizeof(circuit_t)) failed\n");
		abort();
	}

	c->num_inputs = inputs;
	c->inputs = malloc(sizeof(input_t *) * inputs);
	for (size_t i = 0; i < inputs; ++i) {
		c->inputs[i] = input_new();
	}

	c->num_outputs = outputs;
	c->outputs = malloc(sizeof(output_t *) * outputs);
	for (size_t i = 0; i < outputs; ++i) {
		c->outputs[i] = output_new();
	}

	return c;
}

output_t * circuit_output(circuit_t * c, size_t index) {
	if (index >= c->num_outputs) {
		fprintf("error: circuit_output(): invalid index %llu for circuit %p output\n", (long long unsigned int) index, (void *) c);
		abort();
	}

	return c->outputs[index];
}

input_t * circuit_input(circuit_t * c, size_t index) {
	if (index >= c->num_inputs) {
		fprintf("error: circuit_input(): invalid index %llu for circuit %p input\n", (long long unsigned int) index, (void *) c);
		abort();
	}

	return c->inputs[index];
}

gate_t * gate_new(unsigned char type) {
	gate_t * gate = malloc(sizeof(gate_t));
	if (gate == NULL) {
		fprintf(stderr, "error: gate_new(): malloc(sizeof(gate_t)) failed\n");
		abort();
	}

	gate->type = TYPE_GATE;
	gate->gate = type;
	gate->left = NULL; gate->right = NULL;
	gate->signal = LOW;

	return gate;
}

constant_t * constant_new(bool signal) {
	constant_t * c = malloc(sizeof(constant_t));
	if (c == NULL) {
		fprintf(stderr, "error: constant_new(): malloc(sizeof(constant_t)) failed\n");
		abort();
	}

	c->type = TYPE_CONSTANT;
	c->signal = signal;

	return c;
}

void link_to_input(input_t * i, generic_t * g) {
	switch (g->type) {
		case TYPE_INPUT:
		case TYPE_GATE:
		case TYPE_CONSTANT:
		case TYPE_OUTPUT:
			i->left = (void *) g;
			break;
		default:
			fprintf(stderr, "error: link_to_input(): can't link type %llu with input %p\n", (long long unsigned int) g->type, (void *) i);
			abort();
	}
}

void link_to_gate(gate_t * gate, generic_t * g) {
	switch (g->type) {
		case TYPE_GATE:
			if (gate->gate == GATE_NOT) {
				if (gate->left != NULL) {
					fprintf(stderr, "error: link_to_gate(): left node is already linked on the not gate %p\n", (void *) gate);
					abort();
				}

				gate->left = (void *) g;
				break;
			}

			if (gate->left != NULL) {
				if (gate->right != NULL) {
					fprintf(stderr, "error: link_to_gate(): all nodes already linked on gate %p\n", (void *) gate);
					abort();
				}

				gate->right = (void *) g;
				break;
			}

			gate->left = (void *) g;
			break;
		case TYPE_INPUT:
		case TYPE_CONSTANT:
		case TYPE_OUTPUT:
			if (gate->left != NULL) {
				if (gate->right != NULL) {
					fprintf(stderr, "error: link_to_gate(): all nodes are already linked on the gate %p\n", (void *) gate);
					abort();
				}

				gate->right = (void *) g;
				break;
			}

			gate->left = (void *) g;
			break;
		default:
			fprintf(stderr, "error: link_to_gate(): can't link unknown type %llu with gate %p\n", (long long unsigned int) g->type, (void *) g);
			abort();
	}
}

void link_to_output(output_t * o, generic_t * g) {
	switch (g->type) {
		case TYPE_INPUT:
		case TYPE_GATE:
		case TYPE_CONSTANT:
		case TYPE_OUTPUT:
			if (o->left != NULL) {
				fprintf(stderr, "error: link_to_output(): left node is already linked on the circuit output %p\n", (void *) o);
				abort();
			}

			o->left = (void *) g;
			break;
		default:
			fprintf(stderr, "error: link_to_output(): can't link type %llu with circuit output %p\n", (long long unsigned int) g->type, (void *) o);
			abort();
	}
}

void link_to_circuit_output(circuit_t * c, size_t index, generic_t * g) {
	if (index >= c->num_outputs) {
		fprintf(stderr, "error: link_to_circuit_output(): invalid index to link generic %p to output of circuit %p\n", (void *) g, (void *) c);
		abort();
	}

	link_to_output(c->outputs[index], g);
}

void link_with_circuit_output(circuit_t * c, size_t index, generic_t * g) {
	if (index >= c->num_outputs) {
		fprintf(stderr, "error: link_to_circuit_output(): invalid index to link generic %p to output of circuit %p\n", (void *) g, (void *) c);
		abort();
	}

	switch (g->type) {
		case TYPE_INPUT:
			link_to_input((input_t *) g, (generic_t *) c->outputs[index]);
			break;
		case TYPE_GATE:
			link_to_gate((gate_t *) g, (generic_t *) c->outputs[index]);
			break;
		case TYPE_CONSTANT:
			fprintf(stderr, "error: link_with_circuit_output(): can't link an output to a constant type lol\n");
			abort();
		case TYPE_OUTPUT:
			fprintf(stderr, "error: link_with_circuit_output(): can't link an output to an output type lol\n");
			abort();
		default:
			fprintf(stderr, "error: link_to_output(): can't link type %llu with circuit output %p\n", (long long unsigned int) g->type, (void *) c->outputs[index]);
			abort();
	}
}

void link_to_circuit_input(circuit_t * c, size_t index, generic_t * g) {
	if (index >= c->num_inputs) {
		fprintf(stderr, "error: link_to_circuit_input(): invalid index to link generic %p to output of circuit %p\n", (void *) g, (void *) c);
		abort();
	}

	link_to_input(c->inputs[index], g);
}

int main() {
	circuit_t * circuit = circuit_new(3, 2);
	gate_t * xor = gate_new(GATE_XOR);
	gate_t * xor2 = gate_new(GATE_XOR);
	gate_t * and = gate_new(GATE_AND);
	gate_t * and2 = gate_new(GATE_AND);
	gate_t * or = gate_new(GATE_OR);
	input_t * ia = circuit_input(circuit, 0);
	input_t * ib = circuit_input(circuit, 1);
	input_t * ic = circuit_input(circuit, 2);
	constant_t * a = constant_new(1);
	constant_t * b = constant_new(1);
	constant_t * c = constant_new(1);

	link_to_input(ia, a);
	link_to_input(ib, b);
	link_to_input(ic, c);

// 1st half adder
	link_to_gate(and, ia);
	link_to_gate(and, ib);

	link_to_gate(xor, ia);
	link_to_gate(xor, ib);

// 2nd half adder
	link_to_gate(xor2, xor);
	link_to_gate(xor2, ic);

	link_to_gate(and2, xor);
	link_to_gate(and2, ic);

// or
	link_to_gate(or, and);
	link_to_gate(or, and2);

// link to outputs
	link_to_circuit_output(circuit, 0, xor2);
	link_to_circuit_output(circuit, 1, or);

	printf("%u%u\n", evaluate(circuit->outputs[1]), evaluate(circuit->outputs[0]));

	return 0;
}
