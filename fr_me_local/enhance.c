#include <math.h>
#include <stdio.h>

#include "config.h"
#include "enhance.h"
#include "utils.h"

int normalize(unsigned char **image, long length, long width, float image_mean, float image_variance) {
    if (VERBOSE) {
        printf("Starting image normalization.\n");
    }

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

int increase_contrast(unsigned char **image, long length, long width) {
    int i;
    int j;
    int k;
    int l;

    char half_block = CONTRAST_BLOCK_SIZE / 2;

    int pixel_pos;
    int intensity;
    float mean;
    int difference = 0;
    int contrasted_intensity = 0;

    // Iterate over the inner pixels of the image, avoiding the outer pixels
    for (i = half_block; i < length - half_block; ++i) {
        for (j = half_block; j < width - half_block; ++j) {
            mean = 0;

            for (k = -half_block; k <= half_block; ++k) {
                for (l = -half_block; l <= half_block; ++l) {
                    pixel_pos = width * (i + k) + (j + l);
                    intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];

                    mean += intensity;
                }
            }

            mean /= CONTRAST_BLOCK_SIZE * CONTRAST_BLOCK_SIZE;

            pixel_pos = width * i + j;
            intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];

            difference = (intensity - (int) mean);

            contrasted_intensity = mean + CONTRAST_CONSTANT * difference;

            if (contrasted_intensity < 0) {
                contrasted_intensity = 0;
            } else if (contrasted_intensity > 255) {
                contrasted_intensity = 255;
            }

            image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = (unsigned char) contrasted_intensity;
        }
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

    increase_contrast(image, length, width);

    if (VERBOSE) {
        printf("Finished image enhancement.\n");
    }

    return 1;
}