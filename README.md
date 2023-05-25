# krisvers' logic simulator

This is a tiny single file C99 digital logic simulator. I eventually plan to implement a simple GUI for ease of use.

## Example of an extensible full adder circuit:

```c
int main(void) {
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

	link_to_input(ia, (generic_t *) a);
	link_to_input(ib, (generic_t *) b);
	link_to_input(ic, (generic_t *) c);

// 1st half adder
	link_to_gate(and, (generic_t *) ia);
	link_to_gate(and, (generic_t *) ib);

	link_to_gate(xor, (generic_t *) ia);
	link_to_gate(xor, (generic_t *) ib);

// 2nd half adder
	link_to_gate(xor2, (generic_t *) xor);
	link_to_gate(xor2, (generic_t *) ic);

	link_to_gate(and2, (generic_t *) xor);
	link_to_gate(and2, (generic_t *) ic);

// or
	link_to_gate(or, (generic_t *) and);
	link_to_gate(or, (generic_t *) and2);

// link to outputs
	link_to_circuit_output(circuit, 0, (generic_t *) xor2);
	link_to_circuit_output(circuit, 1, (generic_t *) or);

	printf("%u%u\n", evaluate((generic_t *) circuit->outputs[1]), evaluate((generic_t *) circuit->outputs[0]));

	return 0;
}
```
