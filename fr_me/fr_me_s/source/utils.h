/*
 * utils.h
 *
 *  Created on: May 6, 2022
 *      Author: dante
 */

#ifndef UTILS_H_
#define UTILS_H_

float CalculateImageMean(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width);
float CalculateImageVariance(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], float mean, unsigned int length, unsigned int width);
float CalculateBlockMean(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width, int block_start_row, int block_start_col);
float CalculateBlockVariance(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width, int block_start_row, int block_start_col, float mean);

int GetRowsOfBlocks(unsigned int length);
int GetColsOfBlocks(unsigned int width);

int GetBlockStartFromIndex(int i, unsigned int length, unsigned int width, int *block_start_row, int *block_start_col);

int SetBlock(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], int block_index, unsigned int image_length, unsigned int image_width, unsigned char intensity);
int CleanBlock(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], int block_index, unsigned int image_length, unsigned int image_width);

int PrintImageArray(unsigned char image_array[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width);

#endif /* UTILS_H_ */
