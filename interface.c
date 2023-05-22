#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <keys.h>
#include <interface.h>

#define I_INIT_CHECK(func_name) if (init != 1) { fprintf(stderr, "Can't run %s as i_init() was never called\n", (*func_name != '\0') ? func_name : "UNKNOWN"); abort(); }

static char init = 0;

static void error_callback(int error, const char * desc) {
	fprintf(stderr, "GLFW Error %i: %s\n", error, desc);
}

void i_init() {
	if (glfwInit() == 0) {
		fprintf(stderr, "GLFW failed to initialize\n");
		abort();
	}

	glfwSetErrorCallback(error_callback);

	init = 1;
}

window_t * i_window_new(unsigned int w, unsigned int h, unsigned int tw, unsigned int th, char * name) {
	window_t * win;

	I_INIT_CHECK("i_window_new()");

	win = malloc(sizeof(window_t));
	if (win == NULL) {
		fprintf(stderr, "Unable to allocate memory for window\n");
		glfwTerminate();
		abort();
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	win->gwin = (void *) glfwCreateWindow(w, h, name, NULL, NULL);
	if (win->gwin == NULL) {
		fprintf(stderr, "GLFW failed to create window\n");
		glfwTerminate();
		abort();
	}

	glfwSetKeyCallback((GLFWwindow *) win->gwin, key_input);
	glfwMakeContextCurrent((GLFWwindow *) win->gwin);
	glfwSwapInterval(1);
	glfwGetFramebufferSize((GLFWwindow *) win->gwin, &win->w, &win->h);

	win->tws = (unsigned int) win->w / tw;
	win->ths = (unsigned int) win->h / th;
	win->fb = malloc(win->w * win->h * 3);
	if (win->fb == NULL) {
		fprintf(stderr, "Unable to allocate memory for framebuffer\n");
		glfwTerminate();
		abort();
	}

	return win;
}

void i_exit() {
	I_INIT_CHECK("i_exit()");

	glfwTerminate();
}

void i_window_free(window_t * win) {
	if (win == NULL) { return; }

	free(win->fb);
	glfwDestroyWindow((GLFWwindow *) win->gwin);
	free(win);
}

void i_update() {
	I_INIT_CHECK("i_update()");

	glfwPollEvents();
}
