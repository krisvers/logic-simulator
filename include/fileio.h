#ifndef FILEIO_H
#define FILEIO_H

#include <stddef.h>

unsigned char * fileio_load_file(unsigned char * ptr, const char * filename);
size_t fileio_size(const char * filename);

#endif
