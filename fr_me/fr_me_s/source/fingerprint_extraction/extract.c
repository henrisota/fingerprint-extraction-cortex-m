/*
 * extract.c
 *
 *  Created on: Apr 23, 2022
 *      Author: dante
 */
#include <stdio.h>
#include "fsl_debug_console.h"

#include "../config.h"

#include "extract.h"
#include "../secret/secret.h"
#include "../data.h"

struct fingerprint_image_t finger;

unsigned int LoadFingerprintImage(unsigned char fingerprint[][10], unsigned int length, unsigned int width, char *key) {
	if (SHOW_DEBUG) {
	    PRINTF("Loading fingerprint image in the secure world...\n\n");

	    PRINTF("Checking the provided key against the stored key...\n");
	}

    // Verify if the provided key is the correct on to continue loading the fingerprint
    if (VerifyKey(key)) {
    	if (SHOW_DEBUG) {
            PRINTF("Provided key matches the stored key\n\n");

            PRINTF("Storing fingerprint image data on the secure world...\n");
    	}

        memcpy(&(finger.data), fingerprint, length * width);

        if (SHOW_DEBUG) {
            int i;
            int j;

			for (j = 0; j < width; ++j) {
				PRINTF("%d ", finger.data[0][j]);
			}
			PRINTF("\n");

			PRINTF("...\n");

			for (j = 0; j < width; ++j) {
				PRINTF("%d ", finger.data[1][j]);
			}
			PRINTF("\n");
        }

        return 1;
    }

    PRINTF("Provided key does not match the stored key\n");
    PRINTF("Stopping fingerprint loading procedure\n");

    return 0;
}

unsigned int ExtractMinutiae(unsigned int length, unsigned int width) {
	if (SHOW_DEBUG) {
	    PRINTF("Extracting the minutiae from the fingerprint image in the secure world...\n\n");
	}


}

