/*
 * enhance.c
 *
 *  Created on: May 6, 2022
 *      Author: dante
 */
#include <math.h>

#include "fsl_debug_console.h"

#include "../config.h"
#include "../utils.h"
#include "enhance.h"

int Normalize(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width, float image_mean, float image_variance) {
    int i;
    int j;

    if (VERBOSE) {
        PRINTF("Starting image normalization.\r\n");
    }

    for (i = 0; i < length; ++i) {
        for (j = 0; j < width; ++j) {
            int pixel_pos = width * i + j;
            unsigned char intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];

            float scaled_variance = sqrtf(NORMALIZATION_DESIRED_VARIANCE * ((int) intensity - image_mean) * ((int) intensity - image_mean) / image_variance);

            if ((int) intensity > (int) image_mean) {
                image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = (int) (NORMALIZATION_DESIRED_MEAN + scaled_variance) > 255 ? 255 : (int) (NORMALIZATION_DESIRED_MEAN + scaled_variance);
            } else {
                image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = (int) (NORMALIZATION_DESIRED_MEAN - scaled_variance) < 0 ? 0 : (int) (NORMALIZATION_DESIRED_MEAN - scaled_variance);
            }
        }
    }

    if (VERBOSE) {
        PRINTF("Finished image normalization.\r\n");
    }

    return 1;
}

int IncreaseContrast(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width) {
    int i, j, k, l;

    char half_block = CONTRAST_BLOCK_SIZE / 2;

    int pixel_pos;
    unsigned char intensity;
    float mean;
    int difference = 0;
    int contrasted_intensity = 0;

    if (VERBOSE) {
        PRINTF("Starting contrast enhancement.\r\n");
    }

    // Iterate over the inner pixels of the image, avoiding the outer pixels
    for (i = half_block; i < length - half_block; ++i) {
        for (j = half_block; j < width - half_block; ++j) {
            mean = 0;

            for (k = -half_block; k <= half_block; ++k) {
                for (l = -half_block; l <= half_block; ++l) {
                    pixel_pos = width * (i + k) + (j + l);
                    intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];

                    mean += (int) intensity;
                }
            }

            mean /= CONTRAST_BLOCK_SIZE * CONTRAST_BLOCK_SIZE;

            pixel_pos = width * i + j;
            intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];

            difference = ((int) intensity - (int) mean);

            contrasted_intensity = mean + CONTRAST_CONSTANT * difference;

            if (contrasted_intensity < 0) {
                contrasted_intensity = 0;
            } else if (contrasted_intensity > 255) {
                contrasted_intensity = 255;
            }

            image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = (unsigned char) contrasted_intensity;
        }
    }

    if (VERBOSE) {
        PRINTF("Finished contrast enhancement.\r\n");
    }

    return 1;
}

int Enhance(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width) {
    if (VERBOSE) {
        PRINTF("Starting image enhancement.\r\n");
    }

    float image_mean = CalculateImageMean(image, length, width);
    float image_variance = CalculateImageVariance(image, length, width, image_mean);

    if (VERBOSE) {
        if (VERBOSE == 2) {
            PRINTF("Image mean: %d | Image variance: %d\r\n", (int) image_mean, (int) image_variance);
        }
    }

    Normalize(image, length, width, image_mean, image_variance);

    IncreaseContrast(image, length, width);

    if (VERBOSE) {
        PRINTF("Finished image enhancement.\r\n");
    }

    return 1;
}
