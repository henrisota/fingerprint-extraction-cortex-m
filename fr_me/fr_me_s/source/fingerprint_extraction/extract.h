/*
 * extract.h
 *
 *  Created on: May 6, 2022
 *      Author: dante
 */

#ifndef FINGERPRINT_EXTRACTION_EXTRACT_H_
#define FINGERPRINT_EXTRACTION_EXTRACT_H_

#include "../data.h"
#include "../config.h"

int Extract(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width, unsigned char *segmentation_array, int segmentation_array_size, unsigned char *orientation_array, int *orientation_array_size);

#endif /* FINGERPRINT_EXTRACTION_EXTRACT_H_ */
