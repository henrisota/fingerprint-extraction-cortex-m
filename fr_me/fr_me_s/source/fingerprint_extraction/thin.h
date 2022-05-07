/*
 * thin.h
 *
 *  Created on: May 7, 2022
 *      Author: dante
 */

#ifndef FINGERPRINT_EXTRACTION_THIN_H_
#define FINGERPRINT_EXTRACTION_THIN_H_

#include "../data.h"
#include "../config.h"

int Thin(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_LENGTH], unsigned int length, unsigned int width, unsigned char *segmentation_array, int segmentation_array_size);

#endif /* FINGERPRINT_EXTRACTION_THIN_H_ */
