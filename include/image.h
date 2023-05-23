#ifndef IMAGE_H
#define IMAGE_H

typedef struct bmp_header_t {
	char id[2];
	unsigned int size;
	unsigned short rsrv_a;
	unsigned short rsrv_b;
	unsigned int offset;
} bmp_header_t;

typedef struct bmp_dib_t {
	unsigned int dib_size;
	unsigned short bmp_width;
	unsigned short bmp_height;
	unsigned short color_planes;
	unsigned short bpp;
} bmp_dib_t;

typedef struct bmp_t {
	bmp_header_t header;
	bmp_dib_t dib;
	byte * bitmap;
} bmp_t;

#endif
