#include <keys.h>
#include <global.h>
#include <stdio.h>

void key_input(GLFWwindow * window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE) {
		global_close();
	}

	printf("%p, %i, %i, %i, %i\n", (void *) window, key, scancode, action, mods);
}
