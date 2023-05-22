#ifndef UI_H
#define UI_H

typedef unsigned char byte;

typedef struct rect_t {
	unsigned int w;
	unsigned int h;
	unsigned int x;
	unsigned int y;
	unsigned int color;
} rect_t;

typedef struct textured_obj_t {
	byte * tex;
} textured_obj_t;

typedef struct textured_rect_t {
	byte * tex;
	unsigned int w;
	unsigned int h;
	unsigned int x;
	unsigned int y;
} textured_rect_t;

rect_t * ui_rect_new(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color);
void ui_rect_draw(rect_t * rect);
textured_rect_t * ui_textured_rect_new(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
void ui_textured_rect_draw(textured_rect_t * rect);
void ui_textured_obj_copy_tex(textured_obj_t * obj, unsigned int w, unsigned int h, byte * tex);
void ui_textured_obj_load_tex(textured_obj_t * obj, byte * tex);
void ui_textured_obj_free_tex(textured_obj_t * obj);
void ui_textured_obj_free(textured_obj_t * obj);
byte * ui_texture_new(unsigned int w, unsigned int h);
byte * ui_texture_new_from_file(unsigned int w, unsigned int h, const char * filename);

#endif
