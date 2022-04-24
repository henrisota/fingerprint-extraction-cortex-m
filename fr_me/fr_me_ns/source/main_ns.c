/*
 * Copyright 2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_device_registers.h"
#include "fsl_power.h"
#include "pin_mux.h"
#include "veneer_table.h"

#include "data.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define PRINTF_NSE DbgConsole_Printf_NSE
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void SystemInit(void) {
}
/*!
 * @brief Main function
 */
int main(void) {
    unsigned int result = 0;

    /* set BOD VBAT level to 1.65V */
    POWER_SetBodVbatLevel(kPOWER_BodVbatLevel1650mv, kPOWER_BodHystLevel50mv,
                          false);

    // static unsigned char fingerprint[LENGTH][TRANSFER_ARRAY_WIDTH] =
    PRINTF_NSE("Normal World\r\n");

    // Key is used to authorize non-secure application and to proceed with loading
    char *key = "1048576";

    int b = 0;

    int i;
    int j;

    for (i = 0; i < 10; ++i) {
    	for (j = 0; j < 2; ++j) {
    		b = fingerprint[i][j];
    		++b;
    		++b;
    	}
    }

    // Load the fingerprint image into the secure world and confirm if it was successfully loaded
    if (LoadFingerprintImage_NSE(fingerprint, LENGTH, WIDTH, key)) {
        PRINTF_NSE("Loaded image\r\n");
    }

    PRINTF_NSE("Going into looping state...\r\n");

    while (1) {
    }
}
