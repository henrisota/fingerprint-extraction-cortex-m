#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "imageio.h"
#include "preprocess.h"
#include "segment.h"
#include "utils.h"

int preprocess(unsigned char **image, long length, long width, unsigned char **segmentation_array, int *segmentation_array_size) {
    int success = 0;

    *segmentation_array_size = 0;

    if (VERBOSE) {
        printf("Starting preprocessing.\n");
    }

    *segmentation_array = segment(image, length, width, segmentation_array_size);

    if (VERBOSE) {
        printf("Finished preprocessing.\n");
    }

    print_image_array(image, length, width, OUTPUT_TO_FILE);

    success = 1;

    return success;
}