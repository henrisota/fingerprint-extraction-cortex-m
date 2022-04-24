/*
 * secret.c
 *
 *  Created on: Apr 23, 2022
 *      Author: dante
 */
#include "secret.h"

#define STORED_KEY_LENGTH 7

static char storedKey[7] = "1048576";

unsigned int GetSecret() {
    return 42;
}

unsigned int VerifyKey(char *key) {
    if (strnlen(key, 8) != STORED_KEY_LENGTH) {
        return 0;
    }

    for (int i = 0; i < STORED_KEY_LENGTH; i++) {
        if (key[i] != storedKey[i]) {
            return 0;
        }
    }

    return 1;
}
