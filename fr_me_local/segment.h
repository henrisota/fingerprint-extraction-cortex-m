#ifndef _SEGMENT_H
#define _SEGMENT_H

unsigned char *segment(unsigned char **image, long length, long width, int *segmentation_array_size);
int print_segmentation_array(unsigned char *segmentation_array, int segmentation_array_size, int image_length, int image_width);

#endif