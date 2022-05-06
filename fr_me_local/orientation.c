#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "orientation.h"
#include "utils.h"

#define P1_X -1
#define P1_Y -1
#define P2_X -1
#define P2_Y 0
#define P3_X -1
#define P3_Y 1
#define P4_X 0
#define P4_Y -1
#define P5_X 0
#define P5_Y 0
#define P6_X 0
#define P6_Y 1
#define P7_X 1
#define P7_Y -1
#define P8_X 1
#define P8_Y 0
#define P9_X 1
#define P9_Y 1

const char horizontal_sobel_row_positions[6] = {P1_X, P2_X, P3_X, P7_X, P8_X, P9_X};
const char horizontal_sobel_col_positions[6] = {P1_Y, P2_Y, P3_Y, P7_Y, P8_Y, P9_Y};

const char vertical_sobel_row_positions[6] = {P1_X, P4_X, P7_X, P3_X, P6_X, P9_X};
const char vertical_sobel_col_positions[6] = {P1_Y, P3_Y, P4_Y, P6_Y, P7_Y, P9_Y};

const char sobel_coefficients[6] = {1, 2, 1, -1, -2, -1};

unsigned char *create_orientation_array(int no_blocks_rows, int no_blocks_cols, int *orientation_array_size) {
    *orientation_array_size = no_blocks_rows * no_blocks_cols;

    unsigned char *orientation_array = malloc(*orientation_array_size * sizeof(unsigned char));

    if (orientation_array == NULL) {
        printf("malloc of orientation_array failed\n");
        exit(1);
    }

    return orientation_array;
}

int calculate_block_orientation(unsigned char **image, long length, long width, int block_start_row, int block_start_col) {
    // Keep track of block orientation in degrees
    float block_orientation = 0;
    float angle_rad = 0;
    int curr_block_length = BLOCK_ROW_SIZE;
    int curr_block_width = BLOCK_COL_SIZE;
    int i, j, d;
    int pixel_pos, neighbor_pixel_pos;
    unsigned char intensity;
    // Keep track of the gradients
    int g_x = 0, g_y = 0;

    if (length - block_start_row < BLOCK_ROW_SIZE) {
        curr_block_length = length - block_start_row;
    }

    if (width - block_start_col < BLOCK_COL_SIZE) {
        curr_block_width = width - block_start_col;
    }

    // Iterate over all inner pixels of the block
    for (i = block_start_row + 1; i < block_start_row + curr_block_length - 1; ++i) {
        for (j = block_start_col + 1; j < block_start_col + curr_block_width - 1; ++j) {
            g_x = 0;
            g_y = 0;
            pixel_pos = width * i + j;

            // Convolve with horizontal operator
            for (d = 0; d < 6; ++d) {
                neighbor_pixel_pos = pixel_pos + width * horizontal_sobel_row_positions[d] + horizontal_sobel_col_positions[d];
                intensity = image[neighbor_pixel_pos / MAX_IMAGE_WIDTH][neighbor_pixel_pos % MAX_IMAGE_WIDTH];
                g_x += (256 - intensity) * sobel_coefficients[d];
            }

            if (g_x == 0) {
                block_orientation += 90;
                continue;
            }

            // Convolve with vertical operator
            for (d = 0; d < 6; ++d) {
                neighbor_pixel_pos = pixel_pos + width * vertical_sobel_row_positions[d] + vertical_sobel_col_positions[d];
                intensity = image[neighbor_pixel_pos / MAX_IMAGE_WIDTH][neighbor_pixel_pos % MAX_IMAGE_WIDTH];
                g_y += (256 - intensity) * sobel_coefficients[d];
            }

            // Calculate angle using atan2(vertical gradient / horizontal gradient)
            angle_rad = atan2(g_y, g_x);

            // Compute orientation at pixel from atan2(g_y, g_x)
            block_orientation += (int) (angle_rad * 180 / M_PI);
        }
    }

    if ((curr_block_length - 2) <= 0 || (curr_block_width - 2) <= 0) {
        return 90;
    }

    block_orientation /= ((curr_block_length - 2) * (curr_block_width - 2));

    return block_orientation;
}

unsigned char *calculate_local_orientation(unsigned char **image, long length, long width, unsigned char **segmentation_array, int segmentation_array_size, int *orientation_array_size) {
    int i;
    int j;

    if (VERBOSE) {
        printf("Starting calculating orientation of blocks on image.\n");
    }

    int no_blocks_rows = get_rows_of_blocks(length);
    int no_blocks_cols = get_cols_of_blocks(width);
    unsigned char *orientation_array = create_orientation_array(no_blocks_rows, no_blocks_cols, orientation_array_size);

    // Clean up array data
    for (i = 0; i < *orientation_array_size; ++i) {
        orientation_array[i] = 0;
    }

    // Calculate local orientation for segmentation blocks
    for (i = 0; i < segmentation_array_size; ++i) {
        char no_blocks_saved_in_char = 4;

        if (i == segmentation_array_size - 1 && (no_blocks_rows * no_blocks_cols) % 4) {
            no_blocks_saved_in_char = (no_blocks_rows * no_blocks_cols) % 4;
        }

        for (j = 0; j < no_blocks_saved_in_char; ++j) {
            // Create mask at certain position in character to retrieve the
            // segmentation result of the block from the char
            char mask = BLOCK_MASK << (6 - 2 * j);

            if ((mask & (*segmentation_array)[i]) >> (6 - 2 * j) != BACKGROUND_BLOCK) {
                int block_start_row = 0;
                int block_start_col = 0;

                if (VERBOSE) {
                    if (VERBOSE == 2) {
                        printf("Calculation local orientation at block (%d | %d)\n", (i * 4 + j) / no_blocks_cols, (i * 4 + j) % no_blocks_cols);
                    }
                }

                get_block_start_from_index(i * 4 + j, length, width, &block_start_row, &block_start_col);

                int block_orientation = calculate_block_orientation(image, length, width, block_start_row, block_start_col);

                if (VERBOSE) {
                    if (VERBOSE == 2) {
                        printf("Block orientation: %d\n", block_orientation);
                    }
                }

                // Keep the same direction but opposite orientation
                if (block_orientation < -90) {
                    block_orientation += 180;
                } else if (block_orientation > 90) {
                    block_orientation -= 180;
                }

                // Range of block_orientation should be -90 to 90
                // Save into array and add 90 to store properly in unsigned char
                // Converting range to 0 - 180
                orientation_array[i * 4 + j] = block_orientation + 90;
            }
        }
    }

    if (VERBOSE) {
        printf("Finished calculating orientation of blocks on image.\n");
    }

    return orientation_array;
}

int print_orientation_array(unsigned char *orientation_array, int orientation_array_size, int image_length, int image_width) {
    int i;
    int block_counter = 0;
    int no_blocks_cols = get_cols_of_blocks(image_width);

    printf("Orientation array:\n");

    for (i = 0; i < orientation_array_size; ++i) {
        printf("%d ", orientation_array[i]);

        block_counter += 1;

        if (block_counter == no_blocks_cols) {
            printf("\n");
            block_counter = 0;
        }
    }

    printf("\n");

    return 1;
}