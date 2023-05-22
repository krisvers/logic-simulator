#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <interface.h>
#include <global.h>
#include <render.h>
#include <ui.h>
#include <keys.h>
#include <fileio.h>
#include <GLFW/glfw3.h>

#include "CONFIG.h"

#define RGB(value) (value >> 16) & 0xFF, (value >> 8) & 0xFF, (value & 0xFF)

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

window_t * win;

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

int main() {
	i_init();
	win = i_window_new(WIN_WIDTH, WIN_HEIGHT, WIN_TARGET_WIDTH, WIN_TARGET_HEIGHT, "window");
	r_init();
	r_bind_window(win);
	
	for (int w = 0; w < WIN_TARGET_WIDTH; ++w) {
		for (int h = 0; h < WIN_TARGET_HEIGHT; h++) {
			r_draw(w, h, w ^ h);
		}
	}

	byte * texture = ui_texture_new_from_file(24, 16, "./resources/test.raw");

	textured_rect_t * rect = ui_textured_rect_new(24, 16);
	ui_textured_obj_load_tex((textured_obj_t *) rect, (byte *) texture);

	while (!global_should_close() && !glfwWindowShouldClose((GLFWwindow *) win->gwin)) {
		i_update();
		ui_textured_rect_draw(rect);
		r_update();
	}

	i_window_free(win);
	i_exit();

	return 0;
}
