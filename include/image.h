#ifndef IMAGE_H
#define IMAGE_H

typedef enum IMG_TYPES {
	IMG_UNKNOWN = 0,
	IMG_BMP = 1,
} IMG_TYPES;

typedef struct image_t {
	unsigned short type;
	unsigned char * bitmap;
	unsigned int width;
	unsigned int height;
} image_t;

typedef struct bmp_header_t {
	unsigned int size;
	unsigned short rsrv_a;
	unsigned short rsrv_b;
	unsigned int offset;
} bmp_header_t;

typedef struct bmp_dib_t {
	unsigned int dib_size;
	int bmp_width;
	int bmp_height;
	unsigned short color_planes;
	unsigned short bpp;
} bmp_dib_t;

typedef struct bmp_t {
	unsigned short type;
	unsigned char * bitmap;
	unsigned int width;
	unsigned int height;
	bmp_header_t header;
	bmp_dib_t dib;
} bmp_t;

unsigned char * image_load_raw(unsigned char * buffer, char * filename);
bmp_t * image_load_bmp(char * filename);

#endif
