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
    int result;

    /* set BOD VBAT level to 1.65V */
    POWER_SetBodVbatLevel(kPOWER_BodVbatLevel1650mv, kPOWER_BodHystLevel50mv,
                          false);

    PRINTF_NSE("Normal World\r\n");

    result = StringCompare_NSE(&strcmp, "Test1\r\n", "Test2\r\n");
    if (result == 0) {
        PRINTF_NSE("Both strings are equal!\r\n");
    } else {
        PRINTF_NSE("Both strings are not equal!\r\n");
    }
    while (1) {
    }
}
