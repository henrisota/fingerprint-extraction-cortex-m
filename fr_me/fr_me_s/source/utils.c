/*
 * utils.c
 *
 *  Created on: May 6, 2022
 *      Author: dante
 */
#include "config.h"
#include "utils.h"

float CalculateImageMean(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width) {
    float mean = 0;
    int i;
    int j;

    for (i = 0; i < length; ++i) {
        for (j = 0; j < width; ++j) {
            int pixel_pos = width * i + j;
            unsigned char intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];
            mean += (float) intensity;
        }
    }

    mean /= (float) (length * width);

    return mean;
}

float CalculateImageVariance(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], float mean, unsigned int length, unsigned int width) {
    float variance = 0;
    int i;
    int j;

    for (i = 0; i < length; ++i) {
        for (j = 0; j < width; ++j) {
            int pixel_pos = width * i + j;
            unsigned char intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];
            variance += ((int) intensity - (int) mean) * ((int) intensity - (int) mean);
        }
    }

    variance /= (float) (length * width);

    return variance;
}

float CalculateBlockMean(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width, int block_start_row, int block_start_col) {
    int curr_block_length = BLOCK_ROW_SIZE;
    int curr_block_width = BLOCK_COL_SIZE;
    int i;
    int j;
    float mean = 0;

    if (length - block_start_row < BLOCK_ROW_SIZE) {
        curr_block_length = length - block_start_row;
    }

    if (width - block_start_col < BLOCK_COL_SIZE) {
        curr_block_width = width - block_start_col;
    }

    for (i = block_start_row; i < block_start_row + curr_block_length; ++i) {
        for (j = block_start_col; j < block_start_col + curr_block_width; ++j) {
            int pixel_pos = width * i + j;
            unsigned char intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];
            mean += (int) intensity;
        }
    }

    mean /= (curr_block_length * curr_block_width);

    return mean;
}

float CalculateBlockVariance(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width, int block_start_row, int block_start_col, float mean) {
    int curr_block_length = BLOCK_ROW_SIZE;
    int curr_block_width = BLOCK_COL_SIZE;
    int i;
    int j;
    float variance = 0;

    if (length - block_start_row < BLOCK_ROW_SIZE) {
        curr_block_length = length - block_start_row;
    }

    if (width - block_start_col < BLOCK_COL_SIZE) {
        curr_block_width = width - block_start_col;
    }

    for (i = block_start_row; i < block_start_row + curr_block_length; ++i) {
        for (j = block_start_col; j < block_start_col + curr_block_width; ++j) {
            int pixel_pos = width * i + j;
            unsigned char intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];
            variance += ((int) intensity - mean) * ((int) intensity - mean);
        }
    }

    variance /= (curr_block_length * curr_block_width);

    return variance;
}

int GetRowsOfBlocks(unsigned int length) {
    return length / BLOCK_ROW_SIZE + 1 * (length % BLOCK_ROW_SIZE != 0);
}

int GetColsOfBlocks(unsigned int width) {
    return width / BLOCK_COL_SIZE + 1 * (width % BLOCK_COL_SIZE != 0);
}

int GetBlockStartFromIndex(int i, unsigned int length, unsigned int width, int *block_start_row, int *block_start_col) {
    int no_blocks_cols = GetColsOfBlocks(width);

    int block_row_index = i / no_blocks_cols;
    int block_col_index = i % no_blocks_cols;

    *block_start_row = block_row_index * BLOCK_ROW_SIZE;
    *block_start_col = block_col_index * BLOCK_COL_SIZE;

    return 1;
}

int CleanBlock(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], int block_index, unsigned int image_length, unsigned int image_width) {
    return SetBlock(image, block_index, image_length, image_width, 0);
}

int SetBlock(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], int block_index, unsigned int image_length, unsigned int image_width, unsigned char intensity) {
    int i;
    int j;
    int block_start_row = 0;
    int block_start_col = 0;
    int curr_block_length = BLOCK_ROW_SIZE;
    int curr_block_width = BLOCK_COL_SIZE;

    GetBlockStartFromIndex(block_index, image_length, image_width, &block_start_row, &block_start_col);

    if (image_length - block_start_row < BLOCK_ROW_SIZE) {
        curr_block_length = image_length - block_start_row;
    }

    if (image_width - block_start_col < BLOCK_COL_SIZE) {
        curr_block_width = image_width - block_start_col;
    }

    for (i = block_start_row; i < block_start_row + curr_block_length; ++i) {
        for (j = block_start_col; j < block_start_col + curr_block_width; ++j) {
            int pixel_pos = image_width * i + j;
            image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = intensity;
        }
    }

    return 1;
}