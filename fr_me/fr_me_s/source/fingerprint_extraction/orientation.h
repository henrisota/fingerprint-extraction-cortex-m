/*
 * orientation.h
 *
 *  Created on: May 6, 2022
 *      Author: dante
 */

#ifndef FINGERPRINT_EXTRACTION_ORIENTATION_H_
#define FINGERPRINT_EXTRACTION_ORIENTATION_H_

#include "../data.h"
#include "../config.h"

int CalculateLocalOrientation(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width, unsigned char *segmentation_array, int segmentation_array_size, unsigned char *orientation_array, int *orientation_array_size);
int PrintOrientationArray(unsigned char *orientation_array, int orientation_array_size, unsigned int image_length, unsigned int image_width);

#endif /* FINGERPRINT_EXTRACTION_ORIENTATION_H_ */
