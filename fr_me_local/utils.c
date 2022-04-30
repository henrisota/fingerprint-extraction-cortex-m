#include <stdio.h>

#include "config.h"
#include "utils.h"

void print_char_binary(char c) {
    int i;

    for (i = 7; i >= 0; --i) {
        putchar((c & (1 << i)) ? '1' : '0');
    }

    printf("\n");
}

float calculate_image_mean(unsigned char **image, long length, long width) {
    float mean = 0;
    int i;
    int j;

    for (i = 0; i < length; ++i) {
        for (j = 0; j < width; ++j) {
            int pixel_pos = width * i + j;
            float intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];
            mean += intensity;
        }
    }

    mean /= (float) (length * width);

    return mean;
}

float calculate_image_variance(unsigned char **image, float mean, long length, long width) {
    float variance = 0;
    int i;
    int j;

    for (i = 0; i < length; ++i) {
        for (j = 0; j < width; ++j) {
            int pixel_pos = width * i + j;
            float intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];
            variance += (intensity - mean) * (intensity - mean);
        }
    }

    variance /= (float) (length * width);

    return variance;
}

float calculate_block_mean(unsigned char **image, long length, long width, int block_start_row, int block_start_col) {
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
            float intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];
            mean += intensity;
        }
    }

    mean /= (curr_block_length * curr_block_width);

    return mean;
}

float calculate_block_variance(unsigned char **image, long length, long width, int block_start_row, int block_start_col, float mean) {
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
            float intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];
            variance += (intensity - mean) * (intensity - mean);
        }
    }

    variance /= (curr_block_length * curr_block_width);

    return variance;
}

int get_rows_of_blocks(long length) {
    return length / BLOCK_ROW_SIZE + 1 * (length % BLOCK_ROW_SIZE != 0);
}

int get_cols_of_blocks(long width) {
    return width / BLOCK_COL_SIZE + 1 * (width % BLOCK_COL_SIZE != 0);
}

int get_block_start_from_index(int i, int length, int width, int *block_start_row, int *block_start_col) {
    int no_blocks_cols = get_cols_of_blocks(width);

    int block_row_index = i / no_blocks_cols;
    int block_col_index = i % no_blocks_cols;

    *block_start_row = block_row_index * BLOCK_ROW_SIZE;
    *block_start_col = block_col_index * BLOCK_COL_SIZE;

    return 1;
}

int clean_block(unsigned char **image, int block_index, long image_length, long image_width) {
    return set_block(image, block_index, image_length, image_width, 0);
}

int set_block(unsigned char **image, int block_index, long image_length, long image_width, char intensity) {
    int i;
    int j;
    int block_start_row = 0;
    int block_start_col = 0;
    int curr_block_length = BLOCK_ROW_SIZE;
    int curr_block_width = BLOCK_COL_SIZE;

    get_block_start_from_index(block_index, image_length, image_width, &block_start_row, &block_start_col);

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