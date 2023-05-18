#ifndef GATES_H
#define GATES_H

#include <stdint.h>
#include <stddef.h>

#define HIGH 1
#define LOW 0

typedef unsigned char bool;
typedef void (*logic_func)(void * gate);

typedef struct {
	unsigned char type : 1;
} gate_t;

typedef struct {
	unsigned char type : 1;
	bool ** input;
	size_t inputs;
	logic_func logic;
	bool * output;
	size_t outputs;
} builtin_gate_t;

typedef struct {
	unsigned char type;
	builtin_gate_t * gates;
} blueprint_gate_t;

builtin_gate_t * builtin_gate_instantiate(logic_func logic, size_t inputs, size_t outputs);
blueprint_gate_t * blueprint_gate_instantiate(gate_t * gates);
void gate_simulate(gate_t * gate);
void gate_simulate_builtin(builtin_gate_t * gate);
void gate_simulate_blueprint(blueprint_gate_t * gate);

#endif
