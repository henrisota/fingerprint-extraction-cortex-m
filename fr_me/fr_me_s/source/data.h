/*
 * data.h
 *
 *  Created on: Apr 24, 2022
 *      Author: dante
 */

#ifndef _DATA_H_
#define _DATA_H_

#define LENGTH 300
#define WIDTH  300

struct fingerprint_image_t {
    unsigned int length;
    unsigned int width;
    unsigned char data[LENGTH][WIDTH];
};

#endif /* DATA_H_ */
