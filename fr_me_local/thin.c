#include <stdio.h>

#include "config.h"
#include "imageio.h"
#include "thin.h"
#include "utils.h"

int thin_block_from_index(char block_type, unsigned char **image, int index, long image_length, long image_width) {
    int curr_block_length = BLOCK_ROW_SIZE;
    int curr_block_width = BLOCK_COL_SIZE;
    int i;
    int j;
    int pixel_pos = 0;
    unsigned char intensity = 0;

    char min_pixel_dimension_index = -1;
    unsigned char min_pixel_value = 255;

    char second_min_pixel_dimension_index = -1;
    unsigned char second_min_pixel_value = 255;

    char third_min_pixel_dimension_index = -1;
    unsigned char third_min_pixel_value = 255;

    int block_start_row = 0;
    int block_start_col = 0;

    get_block_start_from_index(index, image_length, image_width, &block_start_row, &block_start_col);

    if (image_length - block_start_row < BLOCK_ROW_SIZE) {
        curr_block_length = image_width - block_start_row;
    }

    if (image_width - block_start_col < BLOCK_COL_SIZE) {
        curr_block_width = image_width - block_start_col;
    }

    if (VERBOSE) {
        if (VERBOSE == 2) {
            printf("Thinning on block (%d | %d).\n", block_start_row, block_start_col);
        }
    }

    if (block_type == TYPE_1_H_BLOCK) {
        if (VERBOSE) {
            if (VERBOSE == 2) {
                printf("Block is of type %d.\n", TYPE_1_H_BLOCK);
            }
        }

        // Iterate through each column
        for (i = block_start_col; i < block_start_col + curr_block_width; ++i) {
            // Ranges in value depending on size of dimension iterated to find minimum on
            min_pixel_dimension_index = -1;
            min_pixel_value = 255;

            second_min_pixel_dimension_index = -1;
            second_min_pixel_value = 255;

            third_min_pixel_dimension_index = -1;
            third_min_pixel_value = 255;

            // Find the top three minimum values in the col
            // Set the first to RIDGE_INTENSITY and set the remaining two to
            // WEAK_RIDGE_INTENSITY
            for (j = block_start_row; j < block_start_row + curr_block_length; ++j) {
                pixel_pos = image_width * j + i;
                intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];

                if (intensity < min_pixel_value) {
                    third_min_pixel_dimension_index = second_min_pixel_dimension_index;
                    second_min_pixel_dimension_index = min_pixel_dimension_index;
                    min_pixel_dimension_index = j - block_start_row;

                    third_min_pixel_value = second_min_pixel_value;
                    second_min_pixel_value = min_pixel_value;
                    min_pixel_value = intensity;
                } else if (intensity > min_pixel_value && intensity < second_min_pixel_value) {
                    third_min_pixel_dimension_index = second_min_pixel_dimension_index;
                    second_min_pixel_dimension_index = j - block_start_row;

                    third_min_pixel_value = second_min_pixel_value;
                    second_min_pixel_value = intensity;
                } else if (intensity > second_min_pixel_value && intensity < third_min_pixel_value) {
                    third_min_pixel_dimension_index = j - block_start_row;

                    third_min_pixel_value = intensity;
                }
            }

            // Set the main ridge pixel, weak ridge pixels and valley pixels for each col
            for (j = block_start_row; j < block_start_row + curr_block_length; ++j) {
                pixel_pos = image_width * j + i;

                if (j - block_start_row == min_pixel_dimension_index) {
                    image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = RIDGE_INTENSITY;
                } else if (j - block_start_row == second_min_pixel_dimension_index || j - block_start_row == third_min_pixel_dimension_index) {
                    image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = WEAK_RIDGE_INTENSITY;
                } else {
                    image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = VALLEY_INTENSITY;
                }
            }
        }
    } else if (block_type == TYPE_2_V_BLOCK) {
        if (VERBOSE) {
            if (VERBOSE == 2) {
                printf("Block is of type %d.\n", TYPE_2_V_BLOCK);
            }
        }

        // Iterate through each row
        for (i = block_start_row; i < block_start_row + curr_block_length; ++i) {
            // Ranges in value depending on size of dimension iterated to find minimum on
            min_pixel_dimension_index = -1;
            min_pixel_value = 255;

            second_min_pixel_dimension_index = -1;
            second_min_pixel_value = 255;

            third_min_pixel_dimension_index = -1;
            third_min_pixel_value = 255;

            // Find the top three minimum values in the row
            // Set the first to RIDGE_INTENSITY and set the remaining two to
            // WEAK_RIDGE_INTENSITY. Set the remaining to VALLEY_INTENSITY
            for (j = block_start_col; j < block_start_col + curr_block_width; ++j) {
                pixel_pos = image_width * i + j;
                intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];

                if (intensity < min_pixel_value) {
                    third_min_pixel_dimension_index = second_min_pixel_dimension_index;
                    second_min_pixel_dimension_index = min_pixel_dimension_index;
                    min_pixel_dimension_index = j - block_start_col;

                    third_min_pixel_value = second_min_pixel_value;
                    second_min_pixel_value = min_pixel_value;
                    min_pixel_value = intensity;
                } else if (intensity > min_pixel_value && intensity < second_min_pixel_value) {
                    third_min_pixel_dimension_index = second_min_pixel_dimension_index;
                    second_min_pixel_dimension_index = j - block_start_col;

                    third_min_pixel_value = second_min_pixel_value;
                    second_min_pixel_value = intensity;
                } else if (intensity > second_min_pixel_value && intensity < third_min_pixel_value) {
                    third_min_pixel_dimension_index = j - block_start_col;

                    third_min_pixel_value = intensity;
                }
            }

            // Set the main ridge pixel, weak ridge pixels and valley pixels for each row
            for (j = block_start_col; j < block_start_col + curr_block_width; ++j) {
                pixel_pos = image_width * i + j;

                if (j - block_start_col == min_pixel_dimension_index) {
                    image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = RIDGE_INTENSITY;
                } else if (j - block_start_col == second_min_pixel_dimension_index || j - block_start_col == third_min_pixel_dimension_index) {
                    image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = WEAK_RIDGE_INTENSITY;
                } else {
                    image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = VALLEY_INTENSITY;
                }
            }
        }
    }

    return 1;
}

int thin(unsigned char **image, long length, long width, unsigned char **segmentation_array, int segmentation_array_size) {
    int i, j;

    if (VERBOSE) {
        printf("Starting thinning of image.\n");
    }

    int no_blocks_rows = get_rows_of_blocks(length);
    int no_blocks_cols = get_cols_of_blocks(width);

    for (i = 0; i < segmentation_array_size; ++i) {
        char no_blocks_saved_in_char = 4;

        if (i == segmentation_array_size - 1 && (no_blocks_rows * no_blocks_cols) % 4) {
            no_blocks_saved_in_char = (no_blocks_rows * no_blocks_cols) % 4;
        }

        for (j = 0; j < no_blocks_saved_in_char; ++j) {
            // Create mask at certain position in character to retrieve the
            // segmentation result of the block from the char
            char mask = BLOCK_MASK << (6 - 2 * j);
            char block_code = (mask & (*segmentation_array)[i]) >> (6 - 2 * j);

            if (block_code != BACKGROUND_BLOCK) {
                thin_block_from_index(block_code, image, i * 4 + j, length, width);
            }
        }
    }

    connect_ridges(image, length, width);

    if (VERBOSE) {
        printf("Finished thinning of image.\n");
    }

    return 1;
}