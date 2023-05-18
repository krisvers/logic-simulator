#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <gates.h>

typedef struct {
	gate_t * gate;
	void * next;
} gate_node_t;

static void * gate_head = NULL;

builtin_gate_t * builtin_gate_instantiate(logic_func logic, size_t inputs, size_t outputs) {
	builtin_gate_t * gate = malloc(sizeof(builtin_gate_t));
	if (gate == NULL) {
		fprintf(stderr, "error: malloc(sizeof(buildin_gate_t)) failed\n");
		abort();
	}

	gate->logic = logic;
	gate->inputs = inputs;
	gate->outputs = outputs;
	gate->input = malloc(sizeof(bool *) * inputs);
	gate->output = malloc(sizeof(bool) * outputs);

	// linked list
}

void gate_simulate(gate_t * gate) {
	if (gate->type == 0) {
		gate_simulate_builtin((builtin_gate_t *) gate);
		return;
	} else if (gate->type == 1) {
		gate_simulate_blueprint((blueprint_gate_t *) gate);
		return;
	}

	fprintf(stderr, "error: gate: %p type %llu is invalid\n", (void *) gate, (long long unsigned int) gate->type);
	abort();
}

void gate_simulate_builtin(builtin_gate_t * gate) {
	if (((gate_t *) gate)->type != 0) {
		fprintf(stderr, "error: gate: %p type %llu is not a builtin\n", (void *) gate, (long long unsigned int) ((gate_t *) gate)->type);
		abort();
	}

	gate->logic((void *) gate);
}

void gate_simulate_blueprint(blueprint_gate_t * gate) {
	if (((gate_t *) gate)->type != 1) {
		fprintf(stderr, "error: gate: %p type %llu is not a blueprint\n", (void *) gate, (long long unsigned int) ((gate_t *) gate)->type);
		abort();
	}

	gate->logic((void *) gate);
}
}
