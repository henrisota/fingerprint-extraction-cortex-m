/*
 * preprocess.c
 *
 *  Created on: May 6, 2022
 *      Author: dante
 */

#include "fsl_debug_console.h"

#include "../data.h"
#include "../config.h"
#include "enhance.h"
#include "preprocess.h"
#include "segment.h"

int Preprocess(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width, unsigned char *segmentation_array, int *segmentation_array_size) {
    if (VERBOSE) {
        PRINTF("Started preprocessing.\r\n");
    }

    *segmentation_array_size = 0;

    Enhance(image, length, width);

    Segment(image, length, width, segmentation_array, segmentation_array_size);

    if (VERBOSE) {
        PRINTF("Finished preprocessing.\r\n");
    }

    PrintSegmentationArray(segmentation_array, *segmentation_array_size, length, width);

    return 1;
}
