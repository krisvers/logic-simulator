#ifndef FILEIO_H
#define FILEIO_H

unsigned char * fileio_load_raw_image(unsigned char * ptr, const char * filename);
void fileio_write_raw_image(unsigned char * ptr, unsigned long long length, const char * filename);

#endif
