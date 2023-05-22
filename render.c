#include <interface.h>
#include <render.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#define R_INIT_CHECK(func_name) if (init != 1) { fprintf(stderr, "Can't run %s as r_init() was never called\n", (*func_name != '\0') ? func_name : "UNKNOWN"); abort(); }
#define R_WIN_CHECK(func_name) if (win == NULL) { fprintf(stderr, "Can't run %s as the current window is not bound\n", (*func_name != '\0') ? func_name : "UNKNOWN"); abort(); }
#define R_FB_CHECK(func_name) if (win->fb == NULL) { fprintf(stderr, "Can't run %s as the current framebuffer does not exist\n", (*func_name != '\0') ? func_name : "UNKNOWN"); abort(); }

static window_t * win;
static char init = 0;

void r_raw_draw(unsigned int x, unsigned int y, unsigned int color) {
	R_INIT_CHECK("r_draw()");
	R_WIN_CHECK("r_draw()");
	R_FB_CHECK("r_draw()");

	y = win->h - y - 1;

	if (x >= (unsigned int) win->w || y >= (unsigned int) win->h) {
		return;
	}

	win->fb[x * 3 + (y * 3 * win->w) + 0] = color >> 16;
	win->fb[x * 3 + (y * 3 * win->w) + 1] = color >> 8;
	win->fb[x * 3 + (y * 3 * win->w) + 2] = color;
}

void r_draw(unsigned int x, unsigned int y, unsigned int color) {
	for (float w = 0; w < win->tws; ++w) {
		for (float h = 0; h < win->ths; ++h) {
			r_raw_draw(x * win->tws + w, y * win->ths + h, color);
		}
	}
}

void r_bind_window(void * window) {
	R_INIT_CHECK("r_bind_window()");

	win = (window_t *) window;
}

void r_update() {
	R_INIT_CHECK("r_update()");
	R_WIN_CHECK("r_update()");
	R_FB_CHECK("r_update()");

	glClear(GL_COLOR_BUFFER_BIT);

	glDrawPixels(win->w, win->h, GL_RGB, GL_UNSIGNED_BYTE, win->fb);

	glfwSwapBuffers(win->gwin);
}

void r_init() {
	init = 1;
}
