/*
 * extract.c
 *
 *  Created on: Apr 23, 2022
 *      Author: dante
 */
#include <stdio.h>
#include "fsl_debug_console.h"

#include "../config.h"

#include "../data.h"
#include "../secret/secret.h"
#include "main.h"
#include "preprocess.h"
#include "extract.h"

// Keep the grayscale image
struct fingerprint_image_t finger;
// Keep the segmentation of the blocks of the image
unsigned char segmentation_array[SEGMENTATION_ARRAY_SIZE] = { 0 };
int segmentation_array_size;
// Keep the orientation of the blocks of the image
unsigned char orientation_array[ORIENTATION_ARRAY_SIZE] = { 0 };
int orientation_array_size;

unsigned int LoadFingerprintImage(unsigned char fingerprint[][10], unsigned int length, unsigned int width, char *key) {
    if (VERBOSE) {
        PRINTF("Loading fingerprint image in the secure world...\r\n\r\n");

        PRINTF("Checking the provided key against the stored key...\r\n");
    }

    // Verify if the provided key is the correct on to continue loading the fingerprint
    if (VerifyKey(key)) {
        if (VERBOSE) {
            PRINTF("Provided key matches the stored key\r\n\r\n");

            PRINTF("Storing fingerprint image data on the secure world...\r\n");
        }

        finger.length = length;
        finger.width = width;
        memcpy(&(finger.data), fingerprint, length * width);

        if (VERBOSE) {
            int i;
            int j;

            for (j = 0; j < width; ++j) {
                PRINTF("%d ", finger.data[j / MAX_IMAGE_WIDTH][j % MAX_IMAGE_WIDTH]);
            }
            PRINTF("\n");

            PRINTF("...\n");

            for (j = 0; j < width; ++j) {
                PRINTF("%d ", finger.data[((length - 1) * width + j) / MAX_IMAGE_WIDTH][((length - 1) * width + j) % MAX_IMAGE_WIDTH]);
            }
            PRINTF("\n");
        }

        return 1;
    }

    PRINTF("Provided key does not match the stored key\r\n");
    PRINTF("Stopping fingerprint loading procedure\r\n");

    return 0;
}

unsigned int ExtractMinutiae(unsigned int length, unsigned int width) {
    if (VERBOSE) {
        PRINTF("Starting extraction of minutiae from the fingerprint image in the secure world.\r\n\r\n");
    }

    Preprocess(finger.data, finger.length, finger.width, segmentation_array, &segmentation_array_size);

    Extract(finger.data, finger.length, finger.width, segmentation_array, segmentation_array_size, orientation_array, &orientation_array_size);

    if (VERBOSE) {
        PRINTF("\r\nFinished extraction of minutiae from the fingerprint image in the secure world.\r\n");
    }
}
