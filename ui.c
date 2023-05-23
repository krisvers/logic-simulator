#include <ui.h>
#include <fileio.h>
#include <render.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

rect_t * ui_rect_new(int x, int y, unsigned int w, unsigned int h, unsigned int color) {
	rect_t * r = malloc(sizeof(rect_t));
	if (r == NULL) {
		fprintf(stderr, "Failed to allocate memory for rectangle\n");
		abort();
	}

	r->x = x;
	r->y = y;
	r->w = w;
	r->h = h;
	r->color = color;

	return r;
}

void ui_rect_draw(rect_t * rect) {
	for (unsigned int w = 0; w < rect->w; ++w) {
		for (unsigned int h = 0; h < rect->h; ++h) {
			r_draw(rect->x + w, rect->y + h, rect->color);
		}
	}
}

textured_rect_t * ui_textured_rect_new(int x, int y, unsigned int w, unsigned int h) {
	textured_rect_t * r = malloc(sizeof(textured_rect_t));
	if (r == NULL) {
		fprintf(stderr, "Failed to allocate memory for textured rectangle\n");
		abort();
	}

	r->x = x;
	r->y = y;
	r->w = w;
	r->h = h;
	r->tex = NULL;

	return r;
}

void ui_textured_obj_copy_tex(textured_obj_t * obj, unsigned int w, unsigned int h, byte * tex) {
	byte * new_tex = malloc(w * h * 3);
	if (new_tex == NULL) {
		fprintf(stderr, "Failed to allocate memory for texture\n");
		abort();
	}

	memcpy(new_tex, tex, w * h * 3);
	obj->tex = new_tex;
}

void ui_textured_obj_load_tex(textured_obj_t * obj, byte * tex) {
	obj->tex = tex;
}

void ui_textured_obj_free_tex(textured_obj_t * obj) {
	free(obj->tex);
}

void ui_textured_obj_free(textured_obj_t * obj) {
	free(obj);
}

void ui_draw_checker_texture(int x, int y, unsigned int w, unsigned int h);

void ui_textured_rect_draw(textured_rect_t * rect) {
	if (rect->tex == NULL) {
		fprintf(stderr, "rect %p has no texture to draw\n", rect->tex);
		
	}

	for (unsigned int w = 0; w < rect->w; ++w) {
		for (unsigned int h = 0; h < rect->h; ++h) {
			unsigned int color = rect->tex[(w + (h * rect->w)) * 3] << 16 | rect->tex[(w + (h * rect->w)) * 3 + 1] << 8 | rect->tex[(w + (h * rect->w)) * 3 + 2];
			if (color == 0xFF00FF) {
				continue;
			}

			r_draw(rect->x + w, rect->y + h, color);
		}
	}	
}

byte * ui_texture_new(unsigned int w, unsigned int h) {
	byte * tex = malloc(w * h * 3);
	if (tex == NULL) {
		fprintf(stderr, "Failed to allocate new texture size (%u, %u)\n", w, h);
		abort();
	}

	return tex;
}

byte * ui_texture_new_from_file(unsigned int w, unsigned int h, const char * filename) {
	byte * tex = ui_texture_new(w, h);
	fileio_load_raw_image(tex, filename);

	return tex;
}
