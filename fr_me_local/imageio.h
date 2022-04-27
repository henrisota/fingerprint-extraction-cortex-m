#ifndef _IMAGEIO_H
#define _IMAGEIO_H

#define OUTPUT_TO_STDOUT 1
#define OUTPUT_TO_FILE   2

unsigned char **read_grayscale_binary_file(const char *filename, long *length, long *width);
int print_image_array(unsigned char **image_array, long length, long width, char output);

#endif