#include <image.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <fileio.h>
#include <string.h>

#define I32_FROM_BYTES(array, index) ((int) (() | () | () | ()))
#define I16_FROM_BYTES(array, index) *((short *) (((uintptr_t) array) + index))
#define U32_FROM_BYTES(array, index) *((unsigned int *) (((uintptr_t) array) + index))
#define U16_FROM_BYTES(array, index) *((unsigned short *) (((uintptr_t) array) + index))

typedef unsigned char byte;

byte * image_load_raw(byte * buffer, char * filename) {
	fileio_load_file(buffer, filename);
	return buffer;
}

bmp_t * image_load_bmp(char * filename) {
	size_t size = fileio_size(filename);
	if (size < 0x20) {
		fprintf(stderr, "File %s is to small to be a BMP\n", filename);
		return NULL;
	}

	byte * file = malloc(size);
	fileio_load_file(file, filename);

	if (file[0] != 'B' || file[1] != 'M') {
		fprintf(stderr, "Invalid BMP file %s\n", filename);
		free(file);
		return NULL;
	}

	bmp_t * bmp = malloc(sizeof(bmp_t));
	if (bmp == NULL) {
		fprintf(stderr, "Failed to allocate memory for BMP structure from file %s\n", filename);
		abort();
	}

	bmp->header.size = U32_FROM_BYTES(file, 0x02);
	bmp->header.rsrv_a = U16_FROM_BYTES(file, 0x06);
	bmp->header.rsrv_b = U16_FROM_BYTES(file, 0x08);
	bmp->header.offset = U32_FROM_BYTES(file, 0x0A);

	bmp->dib.dib_size = U32_FROM_BYTES(file, 0x0E);
	bmp->dib.bmp_width = I32_FROM_BYTES(file, 0x12);
	bmp->dib.bmp_height = I32_FROM_BYTES(file, 0x16);
	bmp->dib.color_planes = U16_FROM_BYTES(file, 0x1A);
	bmp->dib.bpp = U16_FROM_BYTES(file, 0x1C);

	bmp->type = IMG_BMP;
	bmp->width = (unsigned int) bmp->dib.bmp_width;
	bmp->height = (unsigned int) bmp->dib.bmp_height;
	bmp->bitmap = malloc(bmp->width * bmp->height * bmp->dib.bpp / 8);
	if (bmp->bitmap == NULL) {
		fprintf(stderr, "Failed to allocate memory for BMP bitmap from file %s\n", filename);
		abort();
	}

	for (size_t i = 0; i < bmp->width * bmp->height * (bmp->dib.bpp); ++i) {
		bmp->bitmap[i] = file[bmp->header.offset + i];
	}

	return bmp;
}
