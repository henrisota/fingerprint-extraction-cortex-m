#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "extract.h"
#include "imageio.h"
#include "preprocess.h"
#include "segment.h"

int main(int argc, char *argv[]) {
    int i;
    char name[80];

    // Image shape
    long length;
    long width;

    // Keep the grayscale image values
    unsigned char **image;
    // Keep the segmentation of the blocks of the image
    unsigned char *block_segmentation;
    int segmented_image_array_size;

    if (argc < 2) {
        printf("Not enough parameters");
    }

    strcpy(name, argv[1]);

    image = read_grayscale_binary_file(name, &length, &width);

    preprocess(image, length, width, &block_segmentation, &segmented_image_array_size);

    // extract(image, length, width, block_segmentation);

    // Deallocate memory
    free(block_segmentation);

    for (i = 0; i < length; ++i) {
        free(image[i]);
    }

    free(image);

    return 0;
}