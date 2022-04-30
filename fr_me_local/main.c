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
    unsigned char *segmentation_array;
    int segmentation_array_size;
    // Keep the extracted features (minutiae)

    if (argc < 2) {
        printf("Not enough parameters");
    }

    strcpy(name, argv[1]);

    image = read_grayscale_binary_file(name, &length, &width);

    preprocess(image, length, width, &segmentation_array, &segmentation_array_size);

    extract(image, length, width, &segmentation_array, segmentation_array_size);

    // Deallocate memory of the segmentation array
    free(segmentation_array);

    // Deallocate memory of the image
    for (i = 0; i < length; ++i) {
        free(image[i]);
    }

    free(image);

    return 0;
}