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

static unsigned int circuit_id_iter = 0;

char * gate_strings[4] = {
	"not",
	"or",
	"and",
	"xor"
};

#define TYPE_INPUT 0
#define TYPE_GATE 1
#define TYPE_CONSTANT 2
#define TYPE_OUTPUT 3

char * type_strings[4] = {
	"input",
	"gate",
	"constant",
	"output"
};

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
	size_t num;
	unsigned short id;
} input_t;

typedef struct {
	unsigned char type;
	bool signal;
	void * left;
	size_t num;
	unsigned short id;
	void * circuit;
} output_t;

typedef struct {
	size_t num_inputs;
	input_t ** inputs;
	size_t num_outputs;
	output_t ** outputs;
	unsigned short id;
} circuit_t;

typedef struct {
	unsigned char * buffer;
	size_t size;
} file_t;

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

	printf("start\n");
	for (i = 0; i < circuit->num_outputs; ++i) {
		printf("next:\n%p\n", (void *) circuit->outputs);
		printf("%p\n", (void *) circuit->outputs[i]);
		outputs[i] = evaluate((generic_t *) circuit->outputs[i]);
	}
	printf("end\n");

	return outputs;
}

output_t * output_new(void) {
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

input_t * input_new(void) {
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
	c->id = circuit_id_iter;
	c->inputs = malloc(sizeof(input_t *) * inputs);
	for (size_t i = 0; i < inputs; ++i) {
		c->inputs[i] = input_new();
		c->inputs[i]->num = i;
		c->inputs[i]->id = circuit_id_iter;
	}

	c->num_outputs = outputs;
	c->outputs = malloc(sizeof(output_t *) * outputs);
	for (size_t i = 0; i < outputs; ++i) {
		c->outputs[i] = output_new();
		c->outputs[i]->num = i;
		c->outputs[i]->circuit = (void *) c;
		c->outputs[i]->id = circuit_id_iter;
	}

	++circuit_id_iter;

	return c;
}

output_t * circuit_output(circuit_t * c, size_t index) {
	if (index >= c->num_outputs) {
		fprintf(stderr, "error: circuit_output(): invalid index %llu for circuit %p output\n", (long long unsigned int) index, (void *) c);
		abort();
	}

	return c->outputs[index];
}

input_t * circuit_input(circuit_t * c, size_t index) {
	if (index >= c->num_inputs) {
		fprintf(stderr, "error: circuit_input(): invalid index %llu for circuit %p input\n", (long long unsigned int) index, (void *) c);
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
		fprintf(stderr, "error: link_with_circuit_output(): invalid index to link generic %p to output of circuit %p\n", (void *) g, (void *) c);
		abort();
	}

	switch (g->type) {
		case TYPE_INPUT:
			link_to_input((input_t *) g, (generic_t *) circuit_output(c, index));
			break;
		case TYPE_GATE:
			link_to_gate((gate_t *) g, (generic_t *) circuit_output(c, index));
			break;
		case TYPE_CONSTANT:
			fprintf(stderr, "error: link_with_circuit_output(): can't link an output to a constant type lol\n");
			abort();
		case TYPE_OUTPUT:
			link_to_output((output_t *) g, (generic_t *) circuit_output(c, index));
			break;
		default:
			fprintf(stderr, "error: link_with_output(): can't link type %llu with circuit output %p\n", (long long unsigned int) g->type, (void *) c->outputs[index]);
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

generic_t * copy_children(generic_t * g, circuit_t * c, circuit_t * nc);
generic_t * copy_children(generic_t * g, circuit_t * c, circuit_t * nc) {
	if ((void *) g == NULL) {
		return (generic_t *) NULL;
	}

	generic_t * new;
	switch (g->type) {
		case TYPE_INPUT:
			new = (generic_t *) circuit_input(nc, ((input_t *) g)->num);
			((input_t *) new)->left = NULL;
			((input_t *) new)->num = ((input_t *) g)->num;
			return new;
		case TYPE_CONSTANT:
			return (generic_t *) constant_new(((constant_t *) g)->signal);
		case TYPE_GATE:
			new = (generic_t *) gate_new(((gate_t *) g)->gate);
			((gate_t *) new)->left = copy_children((generic_t *) ((gate_t *) g)->left, c, nc);
			((gate_t *) new)->right = copy_children((generic_t *) ((gate_t *) g)->right, c, nc);
			return new;
		case TYPE_OUTPUT:
			new = copy_children((generic_t *) ((output_t *) g)->left, c, nc);
			return new;
		default:
			fprintf(stderr, "error: copy_children(): invalid type to copy\n");
			abort();
	}
}

circuit_t * circuit_new_from_blueprint(circuit_t * c) {
	circuit_t * new = circuit_new(c->num_inputs, c->num_outputs);

	for (size_t o = 0; o < c->num_outputs; ++o) {
		if (c->outputs[o]->left == NULL) {
			fprintf(stderr, "error: circuit_new_from_blueprint(): output node is empty, can't copy\n");
			abort();
		}
		new->outputs[o]->num = o;
		new->outputs[o]->circuit = new;
		link_to_circuit_output(new, o, copy_children((generic_t *) c->outputs[o], c, new));
	}

	return new;
}

circuit_t * halfadder_new(void) {
	circuit_t * c = circuit_new(2, 2);
	input_t * a = circuit_input(c, 0);
	input_t * b = circuit_input(c, 1);
	output_t * s = circuit_output(c, 0);
	output_t * cout = circuit_output(c, 1);
	gate_t * xor = gate_new(GATE_XOR);
	gate_t * and = gate_new(GATE_AND);

	link_to_output(s, (generic_t *) xor);
	link_to_gate(xor, (generic_t *) a);
	link_to_gate(xor, (generic_t *) b);

	link_to_output(cout, (generic_t *) and);
	link_to_gate(and, (generic_t *) a);
	link_to_gate(and, (generic_t *) b);

	return c;
}

circuit_t * fulladder_new(void) {
	circuit_t * circ = circuit_new(3, 2);
	circuit_t * x = halfadder_new();
	circuit_t * y = halfadder_new();
	input_t * a = circuit_input(circ, 0);
	input_t * b = circuit_input(circ, 1);
	input_t * c = circuit_input(circ, 2);
	gate_t * or = gate_new(GATE_OR);

	link_to_circuit_input(x, 0, (generic_t *) a);
	link_to_circuit_input(x, 1, (generic_t *) b);
	link_to_circuit_input(y, 0, (generic_t *) circuit_output(x, 0));
	link_to_circuit_input(y, 1, (generic_t *) c);

	link_to_gate(or, (generic_t *) circuit_output(y, 1));
	link_to_gate(or, (generic_t *) circuit_output(x, 1));

	link_to_circuit_output(circ, 0, (generic_t *) circuit_output(y, 0));
	link_to_circuit_output(circ, 1, (generic_t *) or);

	return circ;
}

size_t fileio_size(FILE * file) {
	size_t size;
	fseek(file, 0L, SEEK_END);
	size = ftell(file);
	fseek(file, 0L, SEEK_SET);

	return size;
}

file_t fileio_load(char * filename) {
	FILE * fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "error: fileio_load(): failed to open file %s in \"r\" mode\n", filename);
		abort();
	}

	size_t size = fileio_size(fp);
	unsigned char * buffer = malloc(size);

	if (fread(buffer, size, 1, fp) != 1) {
		fprintf(stderr, "error: fileio_load(): failure whilst reading from file %s\n", filename);
		abort();
	}

	fclose(fp);

	return (file_t) { buffer, size };
}

void fileio_save(file_t file, char * filename) {
	FILE * fp = fopen(filename, "w");

	if (fwrite(file.buffer, file.size, 1, fp) != 1) {
		fprintf(stderr, "error: fileio_save(): failure whilst writing to file %s\n", filename);
		abort();
	}

	fclose(fp);
}

void component_recurse_save(file_t * file, size_t * index, generic_t * g);
void component_recurse_save(file_t * file, size_t * index, generic_t * g) {
	if (*index + 4 >= file->size) {
		file->buffer = realloc(file->buffer, *index + 1);
		if (file->buffer == NULL) {
			fprintf(stderr, "error: component_recurse_save(): failed to reallocate file buffer to size %llu from %llu\n", (long long unsigned int) *index + 1, (long long unsigned int) file->size);
			abort();
		}
	}

	if (g == NULL) {
		printf("null\n");
		file->buffer[*index + 1] = 0xFF;
		file->buffer[*index] = 0xFF;
		*index += 2;
		return;
	}

	file->buffer[*index] = g->type;
	switch (g->type) {
		case TYPE_INPUT:
		case TYPE_OUTPUT:
			printf("%s %lu\n", type_strings[g->type], ((input_t *) g)->num);
			file->buffer[*index + 1] = (unsigned char) ((input_t *) g)->num;
			file->buffer[*index + 2] = (unsigned char) ((input_t *) g)->id & 0xFF;
			file->buffer[*index + 3] = (unsigned char) (((input_t *) g)->id >> 8) & 0xFF;
			*index += 4;
			component_recurse_save(file, index, (generic_t *) ((input_t *) g)->left);
			return;
		case TYPE_GATE:
			printf("%s %s\n", type_strings[g->type], gate_strings[((gate_t *) g)->gate]);
			file->buffer[*index + 1] = ((gate_t *) g)->gate;
			*index += 2;
			component_recurse_save(file, index, (generic_t *) ((gate_t *) g)->left);
			component_recurse_save(file, index, (generic_t *) ((gate_t *) g)->right);
			return;
		case TYPE_CONSTANT:
			printf("%s %u\n", type_strings[g->type], ((constant_t *) g)->signal);
			//file->buffer[*index] = 0xFF; file->buffer[*index + 1] = 0xFF;
			file->buffer[*index + 1] = ((constant_t *) g)->signal;
			*index += 2;
			return;
		default:
			fprintf(stderr, "error: component_recurse_save(): invalid type %u\n", g->type);
			return;
	}
}

void circuit_save(circuit_t * circuit, char * filename) {
	file_t file;
	file.size = 2;
	file.buffer = malloc(file.size);

	file.buffer[0] = circuit->num_inputs;
	file.buffer[1] = circuit->num_outputs;

	size_t index = 2;

	// implement recursing through the tree and saving the node type along with some metadata and other data
	for (size_t o = 0; o < circuit->num_outputs; ++o) {
		component_recurse_save(&file, &index, (generic_t *) circuit_output(circuit, o));
	}

	printf("debug: circuit %p saved in memory\n", (void *) circuit);

	size_t i;
	for (i = 0; i < index; ++i) {
		printf("%02x", file.buffer[i]);
	}
	putc('\n', stdout);

	file.size = i;
	fileio_save(file, filename);
	printf("debug: circuit %p saved to file %s\n", (void *) circuit, filename);
}

void * component_recurse_load(file_t * file, size_t * index, circuit_t * circuit) {
	if (*index > file->size) {
		fprintf(stderr, "component_recurse_load(): current index %llu is larger than the file buffer size %llu\n", (long long unsigned int) *index, (long long unsigned int) file->size);
		abort();
	}

	if (file->buffer[*index] == 0xFF && file->buffer[*index + 1] == 0xFF) {
		printf("null\n");
		*index += 2;
		return NULL;
	}

	generic_t * new;
	switch (file->buffer[*index] & 3) {
		case TYPE_INPUT:
			printf("%s %u\n", type_strings[file->buffer[*index] & 3], file->buffer[*index + 1]);
			new = (generic_t *) circuit_input(circuit, file->buffer[*index + 1]);
			new->type = file->buffer[*index] & 3;
			((input_t *) new)->num = file->buffer[*index + 1];

			((input_t *) new)->id = (file->buffer[*index + 2]) | (file->buffer[*index + 1] << 8);

			*index += 4;
			((input_t *) new)->left = component_recurse_load(file, index, circuit);

			return (void *) new;
		case TYPE_GATE:
			printf("%s %s\n", type_strings[file->buffer[*index] & 3], gate_strings[file->buffer[*index + 1]]);
			new = (generic_t *) gate_new(file->buffer[*index + 1]);
			new->type = file->buffer[*index] & 3;
			
			*index += 2;
			((gate_t *) new)->left = component_recurse_load(file, index, circuit);
			((gate_t *) new)->right = component_recurse_load(file, index, circuit);

			return (void *) new;
		case TYPE_CONSTANT:
			printf("%s %u\n", type_strings[file->buffer[*index] & 3], file->buffer[*index + 1]);
			new = (generic_t *) constant_new(file->buffer[*index + 1]);
			new->type = file->buffer[*index] & 3;

			*index += 2;

			return (void *) new;
		case TYPE_OUTPUT:
			printf("%s %u\n", type_strings[file->buffer[*index] & 3], file->buffer[*index + 1]);
			new = (generic_t *) circuit_output(circuit, file->buffer[*index + 1]);
			new->type = file->buffer[*index] & 3;
			((output_t *) new)->circuit = circuit;
			((output_t *) new)->num = file->buffer[*index + 1];

			((output_t *) new)->id = (file->buffer[*index + 2]) | (file->buffer[*index + 1] << 8);

			*index += 4;
			((output_t *) new)->left = component_recurse_load(file, index, circuit);

			return (void *) new;
	}

	fprintf(stderr, "error: component_recurse_load(): how the actual fuck did you reach this code?!?!?!?!!?!?!\n");
	abort();
}

circuit_t * circuit_load(char * filename) {
	file_t file = fileio_load(filename);
	circuit_t * circuit = circuit_new(file.buffer[0], file.buffer[1]);

	size_t index = 2;
	output_t * current;

	// implement recursing through the tree and loading the node type along with some metadata and other data then linking it
	for (size_t o = 0; o < circuit->num_outputs; ++o) {
		current = component_recurse_load(&file, &index, circuit);
	}

	return circuit;
}

circuit_t * tmp() {
	circuit_t * new = circuit_new(3, 2);
	gate_t * xor = gate_new(GATE_XOR);
	gate_t * xor2 = gate_new(GATE_XOR);
	gate_t * and = gate_new(GATE_AND);
	gate_t * and2 = gate_new(GATE_AND);
	gate_t * or = gate_new(GATE_OR);
	input_t * a = circuit_input(new, 0);
	input_t * b = circuit_input(new, 1);
	input_t * c = circuit_input(new, 2);
	output_t * x = circuit_output(new, 0);
	output_t * y = circuit_output(new, 1);

	link_to_output(x, (generic_t *) xor2);
	link_to_gate(xor2, (generic_t *) xor);
	link_to_gate(xor2, (generic_t *) c);
	link_to_gate(xor, (generic_t *) a);
	link_to_gate(xor, (generic_t *) b);

	link_to_output(y, (generic_t *) or);
	link_to_gate(or, (generic_t *) and);
	link_to_gate(or, (generic_t *) and2);

	link_to_gate(and, (generic_t *) a);
	link_to_gate(and, (generic_t *) b);
	link_to_gate(and2, (generic_t *) xor);
	link_to_gate(and2, (generic_t *) c);

	return new;
}

void print_node(generic_t * node) {
	if (node == NULL) {
		printf("node is NULL\n");
		return;
	}

	switch (node->type) {
		case TYPE_INPUT:
			printf("%s\t\t(%p)\tid: %u\tnum: %lu\tsignal: %u\tleft: %p\n", type_strings[node->type], (void *) node, ((input_t *) node)->id, ((input_t *) node)->num, node->signal, ((input_t *) node)->left);
			print_node((generic_t *) ((input_t *) node)->left);
			return;
		case TYPE_GATE:
			printf("%s\t\t(%p)\tgate: %u\t\tsignal: %u\tleft: %p\tright %p\n", type_strings[node->type], (void *) node, ((gate_t *) node)->gate, node->signal, ((gate_t *) node)->left, ((gate_t *) node)->right);
			print_node((generic_t *) ((gate_t *) node)->left);
			print_node((generic_t *) ((gate_t *) node)->right);
			return;
		case TYPE_CONSTANT:
			printf("%s\t(%p)\t\t\tsignal: %u\n", type_strings[node->type], (void *) node, node->signal);
			return;
		case TYPE_OUTPUT:
			printf("%s\t\t(%p)\tid: %u\tnum: %lu\tsignal: %u\tleft: %p\n", type_strings[node->type], (void *) node, ((output_t *) node)->id, ((output_t *) node)->num, node->signal, ((output_t *) node)->left);
			print_node((generic_t *) ((output_t *) node)->left);
			return;
	}

	fprintf(stderr, "error: print_node(): invalid type %u\n", node->type);
	abort();
}

int main(void) {
	circuit_t * circuit = fulladder_new();
	constant_t * a = constant_new(1);
	constant_t * b = constant_new(1);
	constant_t * c = constant_new(1);

	link_to_circuit_input(circuit, 0, (generic_t *) a);
	link_to_circuit_input(circuit, 1, (generic_t *) b);
	link_to_circuit_input(circuit, 2, (generic_t *) c);

	printf("out 0:\n");
	print_node((generic_t *) circuit_output(circuit, 0));
	printf("out 1:\n");
	print_node((generic_t *) circuit_output(circuit, 1));

	printf("%u%u%u: %u%u\n", a->signal, b->signal, c->signal, evaluate((generic_t *) circuit_output(circuit, 1)), evaluate((generic_t *) circuit_output(circuit, 0)));

	circuit_save(circuit, "./circuit.krc");
	circuit = circuit_load("./circuit.krc");

	printf("out 0:\n");
	print_node((generic_t *) circuit_output(circuit, 0));
	printf("out 1:\n");
	print_node((generic_t *) circuit_output(circuit, 1));

	printf("\n\nevaluate:\n\n");

	printf("%u%u%u: %u%u\n", a->signal, b->signal, c->signal, evaluate((generic_t *) circuit_output(circuit, 1)), evaluate((generic_t *) circuit_output(circuit, 0)));

	return 0;
}
