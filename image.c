#include <image.h>

typedef unsigned char byte;

byte * image_load_raw(byte * buffer, char * filename) {
	fileio_load_file(buffer, filename);
	return buffer;
}

bmp_t * image_load_bmp(char * filename) {
	size_t size = fileio_size(filename);
	if (size < // minimum size)
	byte * file = malloc;
	fileio_load_file(file, filename);

	if (file[0] != 'B' || file[1] != 'M') {
		fprintf("Invalid BMP file %s\n", filename);
		free(file);
		return NULL;
	}

	
}
