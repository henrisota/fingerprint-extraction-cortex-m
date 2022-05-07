/*
 * thin.c
 *
 *  Created on: May 7, 2022
 *      Author: dante
 */
#include "fsl_debug_console.h"

#include "../data.h"
#include "../config.h"
#include "../utils.h"
#include "thin.h"

#define P2_X -1
#define P2_Y 0
#define P3_X -1
#define P3_Y 1
#define P4_X 0
#define P4_Y 1
#define P5_X 1
#define P5_Y 1
#define P6_X 1
#define P6_Y 0
#define P7_X 1
#define P7_Y -1
#define P8_X 0
#define P8_Y -1
#define P9_X -1
#define P9_Y -1

int HeuristicThinBlockFromIndex(char block_type, unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], int index, unsigned int image_length, unsigned int image_width) {
    int curr_block_length = BLOCK_ROW_SIZE;
    int curr_block_width = BLOCK_COL_SIZE;
    int i, j;
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

    GetBlockStartFromIndex(index, image_length, image_width, &block_start_row, &block_start_col);

    if (image_length - block_start_row < BLOCK_ROW_SIZE) {
        curr_block_length = image_width - block_start_row;
    }

    if (image_width - block_start_col < BLOCK_COL_SIZE) {
        curr_block_width = image_width - block_start_col;
    }

    if (VERBOSE) {
        if (VERBOSE == 2) {
            PRINTF("Thinning on block (%d | %d).\r\n", block_start_row, block_start_col);
        }
    }

    if (block_type == TYPE_1_H_BLOCK) {
        if (VERBOSE) {
            if (VERBOSE == 2) {
                PRINTF("Block is of type %d.\r\n", TYPE_1_H_BLOCK);
            }
        }

        // Iterate through each column
        for (i = block_start_col; i < block_start_col + curr_block_width; ++i) {
            // Ranges in value depending on size of dimension iterated to find
            // minimum on
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

            // Set the main ridge pixel, weak ridge pixels and valley pixels
            // for each col
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
                PRINTF("Block is of type %d.\r\n", TYPE_2_V_BLOCK);
            }
        }

        // Iterate through each row
        for (i = block_start_row; i < block_start_row + curr_block_length; ++i) {
            // Ranges in value depending on size of dimension iterated to find
            // minimum on
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

            // Set the main ridge pixel, weak ridge pixels and valley pixels
            // for each row
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

int ConnectRidges(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width) {
    int i, j, k, l, m, n;
    int pixel_pos;
    int neighbor_pixel_pos;
    int second_neighbor_pixel_pos;
    int count_weak_second_neighbors;
    unsigned char intensity;

    if (VERBOSE) {
        PRINTF("Starting connecting ridges.\r\n");
    }

    // Iterate over all pixels except for those on the outer 2 pixel window
    // frame
    for (i = 2; i < length - 2; ++i) {
        for (j = 2; j < width - 2; ++j) {
            pixel_pos = width * i + j;
            intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];

            // Start connecting on ridge pixels
            if (intensity == RIDGE_INTENSITY) {
                // Iterate over the neighboring pixels
                for (k = -1; k <= 1; ++k) {
                    for (l = -1; l <= 1; ++l) {
                        neighbor_pixel_pos = pixel_pos + width * k + l;
                        intensity = image[neighbor_pixel_pos / MAX_IMAGE_WIDTH][neighbor_pixel_pos % MAX_IMAGE_WIDTH];

                        if (intensity == WEAK_RIDGE_INTENSITY) {
                            count_weak_second_neighbors = 0;

                            // Iterate over the neighbors of the weak ridge
                            // neighbor
                            for (m = -1; m <= 1; ++m) {
                                for (n = -1; n <= 1; ++n) {
                                    second_neighbor_pixel_pos = neighbor_pixel_pos + width * m + n;
                                    intensity = image[second_neighbor_pixel_pos / MAX_IMAGE_WIDTH][second_neighbor_pixel_pos % MAX_IMAGE_WIDTH];

                                    if (intensity == WEAK_RIDGE_INTENSITY) {
                                        ++count_weak_second_neighbors;
                                    }
                                }
                            }

                            if (count_weak_second_neighbors > 1) {
                                image[neighbor_pixel_pos / MAX_IMAGE_WIDTH][neighbor_pixel_pos % MAX_IMAGE_WIDTH] = RIDGE_INTENSITY;
                            }
                        }
                    }
                }
            }
        }
    }

    if (VERBOSE) {
        PRINTF("Finished connecting ridges.\r\n");
    }

    return 1;
}

int CleanWeakRidges(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], long length, long width) {
    int i, j;
    int pixel_pos;
    unsigned char intensity;

    if (VERBOSE) {
        PRINTF("Starting cleaning of weak ridges on the image.\r\n");
    }

    for (i = 0; i < length; ++i) {
        for (j = 0; j < width; ++j) {
            pixel_pos = width * i + j;
            intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];

            if (intensity == WEAK_RIDGE_INTENSITY) {
                image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = VALLEY_INTENSITY;
            }
        }
    }

    if (VERBOSE) {
        PRINTF("Finished cleaning of weak ridges on the image.\r\n");
    }

    return 1;
}

int IterativeZhangSuenBlockThin(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], long length, long width) {
    unsigned char block[ZS_THINNING_BLOCK_ROW_SIZE][ZS_THINNING_BLOCK_COL_SIZE] = { 0 };

    int i, j, k, l, d;
    int bi, bj;
    int pixel_pos;
    int neighbor_pixel_pos;
    int successive_neighbor_pixel_pos;
    unsigned char intensity;
    int iteration_count = 0;
    int group_flag = 0;
    int no_blocks_row = length / ZS_THINNING_BLOCK_ROW_SIZE;
    int no_blocks_col = width / ZS_THINNING_BLOCK_COL_SIZE;
    int block_start_row;
    int block_start_col;

    const int transitions_row_positions[9] = { P2_X, P3_X, P4_X, P5_X, P6_X, P7_X, P8_X, P9_X, P2_X };
    const int transitions_col_positions[9] = { P2_Y, P3_Y, P4_Y, P5_Y, P6_Y, P7_Y, P8_Y, P9_Y, P2_Y };

    const int A_one_odd_iteration_first_group_row_positions[3] = { P2_X, P4_X, P6_X };
    const int A_one_odd_iteration_first_group_col_positions[3] = { P2_Y, P4_Y, P6_Y };

    const int A_one_odd_iteration_second_group_row_positions[3] = { P4_X, P6_X, P8_X };
    const int A_one_odd_iteration_second_group_col_positions[3] = { P4_Y, P6_Y, P8_Y };

    const int A_one_even_iteration_first_group_row_positions[3] = { P2_X, P4_X, P8_X };
    const int A_one_even_iteration_first_group_col_positions[3] = { P2_Y, P4_Y, P8_Y };

    const int A_one_even_iteration_second_group_row_positions[3] = { P2_X, P6_X, P8_X };
    const int A_one_even_iteration_second_group_col_positions[3] = { P2_Y, P6_Y, P8_Y };

    if (VERBOSE) {
        PRINTF("Starting iterative modified Zhang-Suen block thinning on image.\r\n");
    }

    // Keep track of the number of valley to ridge transitions in clockwise
    // direction
    int A;
    int B;

    int count_thinned_pixels = 1;

    while (count_thinned_pixels) {
        ++iteration_count;
        count_thinned_pixels = 0;

        block_start_row = (length - (int) no_blocks_row * ZS_THINNING_BLOCK_ROW_SIZE) / 2;

        for (bi = 0; bi < no_blocks_row; ++bi) {
            block_start_col = (width - (int) no_blocks_col * ZS_THINNING_BLOCK_COL_SIZE) / 2;

            for (bj = 0; bj < no_blocks_col; ++bj) {
                // Clean up block array
                for (i = 0; i < ZS_THINNING_BLOCK_ROW_SIZE; ++i) {
                    for (j = 0; j < ZS_THINNING_BLOCK_COL_SIZE; ++j) {
                        block[i][j] = 0;
                    }
                }

                if (VERBOSE) {
                    if (VERBOSE == 2) {
                        PRINTF("Block start row and col: (%d | %d)\r\n", block_start_row, block_start_col);
                    }
                }

                for (i = block_start_row + 1; i < block_start_row + ZS_THINNING_BLOCK_ROW_SIZE - 1; ++i) {
                    for (j = block_start_col + 1; j < block_start_col + ZS_THINNING_BLOCK_COL_SIZE - 1; ++j) {
                        pixel_pos = width * i + j;
                        intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];

                        // Thin on ridge pixels
                        if (intensity == RIDGE_INTENSITY) {
                            B = 0;

                            // Calculate number of non-zero neighbors of pixel
                            for (k = -1; k <= 1; ++k) {
                                for (l = -1; l <= 1; ++l) {
                                    neighbor_pixel_pos = pixel_pos + width * k + l;
                                    intensity = image[neighbor_pixel_pos / MAX_IMAGE_WIDTH][neighbor_pixel_pos % MAX_IMAGE_WIDTH];

                                    if (intensity == RIDGE_INTENSITY) {
                                        ++B;
                                    }
                                }
                            }

                            if (B >= ZS_THINNING_RIDGE_NEIGHBORS_MIN_THRESHOLD && B <= ZS_THINNING_RIDGE_NEIGHBORS_MAX_THRESHOLD) {
                                A = 0;

                                // Calculate number of valley to ridge transitions
                                // between neighbors in clockwise fashion
                                for (d = 0; d < 8; ++d) {
                                    neighbor_pixel_pos = pixel_pos + width * transitions_row_positions[d] + transitions_col_positions[d];
                                    successive_neighbor_pixel_pos = pixel_pos + width * transitions_row_positions[d + 1] + transitions_col_positions[d + 1];

                                    // Check for valley to ridge transition
                                    if (image[neighbor_pixel_pos / MAX_IMAGE_WIDTH][neighbor_pixel_pos % MAX_IMAGE_WIDTH] == VALLEY_INTENSITY &&
                                        image[successive_neighbor_pixel_pos / MAX_IMAGE_WIDTH][successive_neighbor_pixel_pos % MAX_IMAGE_WIDTH] == RIDGE_INTENSITY) {
                                        ++A;
                                        // Skip the next position due to knowing
                                        // that the successive pixel position will
                                        // be a ridge pixel
                                        ++d;
                                    }
                                }

                                if (A == 1) {
                                    if (iteration_count % 2) {
                                        // Odd iteration
                                        group_flag = 0;

                                        // Check if first group has any valley pixels
                                        for (d = 0; d < 3; ++d) {
                                            neighbor_pixel_pos = pixel_pos + width * A_one_odd_iteration_first_group_row_positions[d] + A_one_odd_iteration_first_group_col_positions[d];

                                            if (image[neighbor_pixel_pos / MAX_IMAGE_WIDTH][neighbor_pixel_pos % MAX_IMAGE_WIDTH] == VALLEY_INTENSITY) {
                                                group_flag = 1;
                                                break;
                                            }
                                        }

                                        // If first group didn't have a valley pixel,
                                        // continue with next iteration
                                        if (!group_flag) {
                                            continue;
                                        }

                                        group_flag = 0;

                                        // Check if second group has any valley pixels
                                        for (d = 0; d < 3; ++d) {
                                            neighbor_pixel_pos = pixel_pos + width * A_one_odd_iteration_second_group_row_positions[d] + A_one_odd_iteration_second_group_col_positions[d];

                                            if (image[neighbor_pixel_pos / MAX_IMAGE_WIDTH][neighbor_pixel_pos % MAX_IMAGE_WIDTH] == VALLEY_INTENSITY) {
                                                group_flag = 1;
                                                break;
                                            }
                                        }

                                        // If second group didn't have a valley pixel,
                                        // continue with next iteration
                                        if (!group_flag) {
                                            continue;
                                        }
                                    } else {
                                        // Even iteration
                                        group_flag = 0;

                                        // Check if first group has any valley pixels
                                        for (d = 0; d < 3; ++d) {
                                            neighbor_pixel_pos = pixel_pos + width * A_one_even_iteration_first_group_row_positions[d] + A_one_even_iteration_first_group_col_positions[d];

                                            if (image[neighbor_pixel_pos / MAX_IMAGE_WIDTH][neighbor_pixel_pos % MAX_IMAGE_WIDTH] == VALLEY_INTENSITY) {
                                                group_flag = 1;
                                                break;
                                            }
                                        }

                                        // If first group didn't have a valley pixel,
                                        // continue with next iteration
                                        if (!group_flag) {
                                            continue;
                                        }

                                        group_flag = 0;

                                        // Check if second group has any valley pixels
                                        for (d = 0; d < 3; ++d) {
                                            neighbor_pixel_pos = pixel_pos + width * A_one_even_iteration_second_group_row_positions[d] + A_one_even_iteration_second_group_col_positions[d];

                                            if (image[neighbor_pixel_pos / MAX_IMAGE_WIDTH][neighbor_pixel_pos % MAX_IMAGE_WIDTH] == VALLEY_INTENSITY) {
                                                group_flag = 1;
                                                break;
                                            }
                                        }

                                        // If second group didn't have a valley pixel,
                                        // continue with next iteration
                                        if (!group_flag) {
                                            continue;
                                        }
                                    }

                                    // Delete pixel if checks are true
                                    block[i - block_start_row][j - block_start_col] = 1;
                                    ++count_thinned_pixels;
                                }
                            }
                        }
                    }
                }

                // Thin based on the marked block in block array
                for (i = 1; i < ZS_THINNING_BLOCK_ROW_SIZE - 1; ++i) {
                    for (j = 1; j < ZS_THINNING_BLOCK_COL_SIZE - 1; ++j) {
                        if (block[i][j] == 1) {
                            pixel_pos = width * (block_start_row + i) + (block_start_col + j);
                            image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = VALLEY_INTENSITY;
                        }
                    }
                }

                if (VERBOSE) {
                    if (VERBOSE == 2) {
                        PRINTF("Thinned %d pixels.\r\n", count_thinned_pixels);
                    }
                }

                block_start_col += ZS_THINNING_BLOCK_COL_SIZE;
            }

            block_start_row += ZS_THINNING_BLOCK_ROW_SIZE;
        }
    }

    if (VERBOSE) {
        PRINTF("Finished iterative Zhang-Suen block thinning on image.\r\n");
    }

    return 1;
}

int Thin(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width, unsigned char *segmentation_array, int segmentation_array_size) {
    int i, j;

    if (VERBOSE) {
        PRINTF("Starting thinning of image.\r\n");
    }

    int no_blocks_rows = GetRowsOfBlocks(length);
    int no_blocks_cols = GetColsOfBlocks(width);

    for (i = 0; i < segmentation_array_size; ++i) {
        char no_blocks_saved_in_char = 4;

        if (i == segmentation_array_size - 1 && (no_blocks_rows * no_blocks_cols) % 4) {
            no_blocks_saved_in_char = (no_blocks_rows * no_blocks_cols) % 4;
        }

        for (j = 0; j < no_blocks_saved_in_char; ++j) {
            // Create mask at certain position in character to retrieve the
            // segmentation result of the block from the char
            char mask = BLOCK_MASK << (6 - 2 * j);
            char block_code = (mask & segmentation_array[i]) >> (6 - 2 * j);

            if (block_code != BACKGROUND_BLOCK) {
                HeuristicThinBlockFromIndex(block_code, image, i * 4 + j, length, width);
            }
        }
    }

    ConnectRidges(image, length, width);

    CleanWeakRidges(image, length, width);

    IterativeZhangSuenBlockThin(image, length, width);

    if (VERBOSE) {
        PRINTF("Finished thinning of image.\r\n");
    }

    return 1;
}
