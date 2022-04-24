/*
 * Copyright 2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _VENEER_TABLE_H_
#define _VENEER_TABLE_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef int (*callbackptr)(char const *s1, char const *s2);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Entry function for debug PRINTF (DbgConsole_Printf)
 *
 * This function provides interface between secure and normal worlds
 * This function is called from normal world only
 *
 * @param s     String to be printed
 *
 */
void DbgConsole_Printf_NSE(char const *s);

/*!
 * @brief Entry function for loading the fingerprint image (LoadFingerprintImage)
 *
 * This function provides interface between secure and normal worlds
 * This function is called from normal world only
 *
 * @param fingerprint Fingerprint data
 * @param length Number of rows of the fingerprint data or the height of the image
 * @param width Number of columns of the fingerprint data or the width of the image
 * @param key Key to be submitted to grant authorization to load the fingerprint
 * @return Positive value if the fingerprint was loaded successfully, otherwise 0
 *
 */
unsigned int LoadFingerprintImage_NSE(unsigned char fingerprint[][10], unsigned int length, unsigned int width, char *key);
#endif /* _VENEER_TABLE_H_ */
