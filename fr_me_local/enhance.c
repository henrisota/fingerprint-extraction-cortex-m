#include <math.h>
#include <stdio.h>

#include "config.h"
#include "enhance.h"
#include "utils.h"

int normalize(unsigned char **image, long length, long width, float image_mean, float image_variance) {
    if (VERBOSE) {
        printf("Starting image normalization.\n");
    }

    printf("Image mean: %f\n", image_mean);
    printf("Image variance: %f\n", image_variance);
    int i;
    int j;

    for (i = 0; i < length; ++i) {
        for (j = 0; j < width; ++j) {
            int pixel_pos = width * i + j;
            float intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];

            float scaled_variance = sqrtf(NORMALIZATION_DESIRED_VARIANCE * (intensity - image_mean) * (intensity - image_mean) / image_variance);

            if (intensity > image_mean) {
                image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = (int) (NORMALIZATION_DESIRED_MEAN + scaled_variance) > 255 ? 255 : (int) (NORMALIZATION_DESIRED_MEAN + scaled_variance);
            } else {
                image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = (int) (NORMALIZATION_DESIRED_MEAN - scaled_variance) < 0 ? 0 : (int) (NORMALIZATION_DESIRED_MEAN - scaled_variance);
            }
        }
    }

    if (VERBOSE) {
        printf("Finished image normalization.\n");
    }

    return 1;
}

int enhance(unsigned char **image, long length, long width) {
    if (VERBOSE) {
        printf("Starting image enhancement.\n");
    }

    float image_mean = calculate_image_mean(image, length, width);
    float image_variance = calculate_image_variance(image, length, width, image_mean);

    normalize(image, length, width, image_mean, image_variance);

    if (VERBOSE) {
        printf("Finished image enhancement.\n");
    }

    return 1;
}