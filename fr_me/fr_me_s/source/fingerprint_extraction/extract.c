/*
 * extract.c
 *
 *  Created on: Apr 23, 2022
 *      Author: dante
 */
#include "fsl_debug_console.h"

#include "extract.h"
#include "../secret/secret.h"

unsigned int LoadFingerprintImage(unsigned char fingerprint[][10], unsigned int length, unsigned int width, char *key) {
    PRINTF("Loading fingerprint image in the secure world...\n\n");

    PRINTF("Checking the provided key against the stored key...\n");

    // Verify if the provided key is the correct on to continue loading the fingerprint
    if (VerifyKey(key)) {
        PRINTF("Provided key matches the stored key\n\n");

        PRINTF("Storing fingerprint image data on the secure world...\n");

        return 1;
    }

    PRINTF("Provided key does not match the stored key\n");
    PRINTF("Stopping fingerprint loading procedure\n");

    return 0;
}
