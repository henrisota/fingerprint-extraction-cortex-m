/*
 * data.h
 *
 *  Created on: Apr 24, 2022
 *      Author: dante
 */

#ifndef _DATA_H_
#define _DATA_H_

#define MAX_IMAGE_LENGTH 300
#define MAX_IMAGE_WIDTH  300

struct fingerprint_image_t {
    unsigned int length;
    unsigned int width;
    unsigned char data[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH];
};

#endif /* DATA_H_ */
