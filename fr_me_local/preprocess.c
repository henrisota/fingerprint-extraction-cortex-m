#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "imageio.h"
#include "preprocess.h"
#include "segment.h"
#include "utils.h"

int preprocess(unsigned char **image, long length, long width, unsigned char **block_segmentation, int *segmented_image_array_size) {
    int success = 0;

    *segmented_image_array_size = 0;

    if (VERBOSE) {
        printf("Starting preprocessing.\n");
    }

    *block_segmentation = segment(image, length, width, segmented_image_array_size);

    if (VERBOSE) {
        printf("Finished preprocessing.\n");
    }

    print_image_array(image, length, width, OUTPUT_TO_FILE);

    success = 1;

    return success;
}