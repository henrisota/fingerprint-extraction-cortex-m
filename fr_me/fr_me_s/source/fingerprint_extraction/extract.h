/*
 * extract.h
 *
 *  Created on: Apr 23, 2022
 *      Author: dante
 */

#ifndef _EXTRACT_H_
#define _EXTRACT_H_

unsigned int LoadFingerprintImage(unsigned char fingerprint[][10], unsigned int length, unsigned int width, char *key);
unsigned int ExtractMinutiae(unsigned int length, unsigned int width);

#endif /* _EXTRACT_H_ */
