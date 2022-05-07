/*
 * extract.h
 *
 *  Created on: Apr 23, 2022
 *      Author: dante
 */

#ifndef FINGERPRINT_EXTRACTION_MAIN_H_
#define FINGERPRINT_EXTRACTION_MAIN_H_

unsigned int LoadFingerprintImage(unsigned char fingerprint[][10], unsigned int length, unsigned int width, char *key);
unsigned int ExtractMinutiae(unsigned int length, unsigned int width);

#endif /* FINGERPRINT_EXTRACTION_MAIN_H_ */
