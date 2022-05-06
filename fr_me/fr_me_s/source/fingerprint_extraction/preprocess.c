/*
 * preprocess.c
 *
 *  Created on: May 6, 2022
 *      Author: dante
 */

#include "fsl_debug_console.h"

#include "../config.h"
#include "preprocess.h"
#include "../data.h"

int Preprocess(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width, unsigned char *segmentation_array, int *segmentation_array_size) {
    if (VERBOSE) {
        PRINTF("Started preprocessing.\r\n");
    }

    int i;
    int j;




    if (VERBOSE) {
        PRINTF("Finished preprocessing.\r\n");
    }

    return 1;
}
