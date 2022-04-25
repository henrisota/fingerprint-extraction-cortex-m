#ifndef _EXTRACT_H
#define _EXTRACT_H

unsigned char **read_grayscale_binary_file(const char *filename, long *length, long *width);
int print_image_array(unsigned char **image_array, long length, long width);

#endif
