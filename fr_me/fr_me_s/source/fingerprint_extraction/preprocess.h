/*
 * preprocess.h
 *
 *  Created on: May 6, 2022
 *      Author: dante
 */

#ifndef FINGERPRINT_EXTRACTION_PREPROCESS_H_
#define FINGERPRINT_EXTRACTION_PREPROCESS_H_

#include "config.h"
#include "../data.h"

int Preprocess(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width, unsigned char *segmentation_array, int *segmentation_array_size);

#endif /* FINGERPRINT_EXTRACTION_PREPROCESS_H_ */
