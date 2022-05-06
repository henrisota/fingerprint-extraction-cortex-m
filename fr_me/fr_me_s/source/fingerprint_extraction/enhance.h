/*
 * enhance.h
 *
 *  Created on: May 6, 2022
 *      Author: dante
 */

#ifndef FINGERPRINT_EXTRACTION_ENHANCE_H_
#define FINGERPRINT_EXTRACTION_ENHANCE_H_

#include "../config.h"

int Enhance(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width);

#endif /* FINGERPRINT_EXTRACTION_ENHANCE_H_ */
