#ifndef _ORIENTATION_H
#define _ORIENTATION_H

unsigned char *calculate_local_orientation(unsigned char **image, long length, long width, unsigned char **segmentation_array, int segmentation_array_size, int *orientation_array_size);
int print_orientation_array(unsigned char *orientation_array, int orientation_array_size, int image_length, int image_width);

#endif
