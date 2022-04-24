/*
 * extract.c
 *
 *  Created on: Apr 23, 2022
 *      Author: dante
 */
#include "fsl_debug_console.h"

#include "extract.h"
#include "../secret/secret.h"

#include "../data.h"

#include <stdio.h>

struct fingerprint_image_t finger;

unsigned int LoadFingerprintImage(unsigned char fingerprint[][10], unsigned int length, unsigned int width, char *key) {
    PRINTF("Loading fingerprint image in the secure world...\n\n");

    PRINTF("Checking the provided key against the stored key...\n");

    // Verify if the provided key is the correct on to continue loading the fingerprint
    if (VerifyKey(key)) {
        PRINTF("Provided key matches the stored key\n\n");

        PRINTF("Storing fingerprint image data on the secure world...\n");

        int copy_size = length * width;
        int left_page_size = 32766;
        int max_copy_round_size = 100 * width;
        int data_pos = 0;

        // while (1) {
        //     if (copy_size == 0)
        //         break;

        //     PRINTF("%d | %d | %d | %d\n", copy_size, left_page_size, max_copy_round_size, data_pos);

        //     if (copy_size < left_page_size) {
        //         left_page_size = copy_size;
        //     }

        //     if (max_copy_round_size < left_page_size) {
        //         left_page_size -= max_copy_round_size;
        //         memcpy(&(finger.data) + data_pos, fingerprint + data_pos, max_copy_round_size);

        //         data_pos += max_copy_round_size;
        //         copy_size -= max_copy_round_size;
        //     } else {
        //         memcpy(&(finger.data) + data_pos, fingerprint + data_pos, left_page_size);

        //         data_pos += left_page_size;
        //         copy_size -= left_page_size;

        //         left_page_size = 32768;
        //     }
        // }
        memcpy(&(finger.data) + data_pos, fingerprint + data_pos, 30000);

        int i;
        int j;

        for (i = 0; i < 1; ++i) {
            for (j = 0; j < width; ++j) {
                PRINTF("%d ", finger.data[i][j]);
            }

            PRINTF("\n");
        }

        PRINTF("data: %d\n", finger.data[0][0]);

        return 1;
    }

    PRINTF("Provided key does not match the stored key\n");
    PRINTF("Stopping fingerprint loading procedure\n");

    return 0;
}
