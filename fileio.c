#include <fileio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

void fileio_write_raw_image(unsigned char * ptr, unsigned long long length, const char * filename) {
	FILE * fp = fopen(filename, "w");
	if (fp == NULL) {
		fprintf(stderr, "Can't open file %s for loading raw image\n", filename);
		abort();
	}

	if (fwrite(ptr, 1, length, fp) != 1) {
		fprintf(stderr, "Failed to write to file %s for saving raw image\n", filename);
		abort();
	}

	fclose(fp);
}

unsigned char * fileio_load_file(unsigned char * ptr, const char * filename) {
	FILE * fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Can't open file %s for loading raw image\n", filename);
		abort();
	}

	fseek(fp, 0L, SEEK_END);
	unsigned long long size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	if (fread(ptr, size, 1, fp) == 0) {
		fprintf(stderr, "Failed to read from file %s for loading raw image\n", filename);
		abort();
	}

	fclose(fp);
	return ptr;
}

size_t fileio_size(const char * filename) {
	FILE * fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf("File %s can't be read from\n", filename);
		return 0;
	}

	fseek(fp, 0L, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	fclose(fp);

	return size;
}
