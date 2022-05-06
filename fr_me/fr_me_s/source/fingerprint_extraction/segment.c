/*
 * segment.c
 *
 *  Created on: May 6, 2022
 *      Author: dante
 */
#include "fsl_debug_console.h"

#include "../data.h"
#include "../config.h"
#include "../utils.h"
#include "segment.h"

int ErodeBlockFromIndex(char block_type, unsigned char *segmentation_array, int index, int no_blocks_rows, int no_blocks_cols) {
    int result = 0;
    char mask;
    int i, j;
    int neighbor_index = 0;

    int no_blocks = no_blocks_rows * no_blocks_cols;

    int count_neighbors = 0;
    int count_neighbor_foreground_blocks = 0;

    // Iterate over neighboring blocks and check if they are foreground blocks
    for (i = -1; i <= 1; ++i) {
        for (j = -1; j <= 1; ++j) {
            if (i != 0 || j != 0) {
                neighbor_index = index + i * no_blocks_cols + j;

                if (neighbor_index >= 0 && neighbor_index < no_blocks) {
                    ++count_neighbors;

                    // Create mask at certain position in character to retrieve the
                    // segmentation result of the block from the char
                    mask = BLOCK_MASK << (6 - 2 * (neighbor_index % 4));

                    if ((mask & segmentation_array[neighbor_index / 4]) >> (6 - 2 * (neighbor_index % 4)) == FOREGROUND_BLOCK) {
                        if (VERBOSE) {
                            if (VERBOSE == 2) {
                                PRINTF("Neighbor with index %d (%d | %d) is foreground\r\n", neighbor_index, neighbor_index / no_blocks_cols, neighbor_index % no_blocks_cols);
                            }
                        }

                        count_neighbor_foreground_blocks += 1;
                    }
                }
            }
        }
    }

    if (VERBOSE) {
        if (VERBOSE == 2) {
            PRINTF("Number of neighboring foreground blocks: %d\r\n", count_neighbor_foreground_blocks);
        }
    }

    // Calculate mask to set the corresponding block on the character
    mask = BLOCK_MASK << (6 - 2 * (index % 4));

    // Mark block as foreground block if the number of foreground neighbor
    // blocks is higher than the threshold or mark block as background block
    // if the number of foreground neighbor blocks is less than the threshold
    if (block_type == BACKGROUND_BLOCK && count_neighbor_foreground_blocks > BACKGROUND_EROSION_FOREGROUND_NEIGHBORS_THRESHOLD) {
        // Set the corresponding 2 bits of the character corresponding to
        // the position of the block
        segmentation_array[index / 4] = (segmentation_array[index / 4] & ~mask) | (FOREGROUND_BLOCK << (6 - 2 * (index % 4)));

        result = 1;
    } else if (block_type == FOREGROUND_BLOCK && count_neighbor_foreground_blocks < FOREGROUND_EROSION_BACKGROUND_NEIGHBORS_THRESHOLD) {
        // Set the corresponding 2 bits of the character corresponding to
        // the position of the block
        segmentation_array[index / 4] = (segmentation_array[index / 4] & ~mask) | (BACKGROUND_BLOCK << (6 - 2 * (index % 4)));

        result = 1;
    }

    return result;
}

int ErodeLoneBlocks(unsigned char *segmentation_array, int segmentation_array_size, int no_blocks_rows, int no_blocks_cols) {
    int iteration = EROSION_ITERATIONS;
    int i, j;
    int result = 0;

    if (VERBOSE) {
        PRINTF("Starting erosion of lone segmentation blocks on image.\r\n");
    }

    while (iteration--) {
        for (i = 0; i < segmentation_array_size; ++i) {
            char no_blocks_saved_in_char = 4;

            if (i == segmentation_array_size - 1 && (no_blocks_rows * no_blocks_cols) % 4) {
                no_blocks_saved_in_char = (no_blocks_rows * no_blocks_cols) % 4;
            }

            for (j = 0; j < no_blocks_saved_in_char; ++j) {
                // Create mask at certain position in character to retrieve the
                // segmentation result of the block from the char
                char mask = BLOCK_MASK << (6 - 2 * j);

                if (VERBOSE) {
                    if (VERBOSE == 2) {
                        PRINTF("Eroding at block (%d | %d)\r\n", (i * 4 + j) / no_blocks_cols, (i * 4 + j) % no_blocks_cols);
                    }
                }

                if ((mask & segmentation_array[i]) >> (6 - 2 * j) == BACKGROUND_BLOCK) {
                    // Apply erosion on the background blocks
                    result = ErodeBlockFromIndex(BACKGROUND_BLOCK, segmentation_array, i * 4 + j, no_blocks_rows, no_blocks_cols);

                    if (result == 0) {
                        if (VERBOSE) {
                            if (VERBOSE == 2) {
                                PRINTF("Couldn't erode background block with index %d.\r\n", i * 4 + j);
                            }
                        }
                    } else if (result == 1) {
                        if (VERBOSE) {
                            if (VERBOSE == 2) {
                                PRINTF("Eroded background block with index %d.\r\n", i * 4 + j);
                            }
                        }
                    }
                } else {
                    // Apply erosion on the foreground blocks
                    result = ErodeBlockFromIndex(FOREGROUND_BLOCK, segmentation_array, i * 4 + j, no_blocks_rows, no_blocks_cols);

                    if (result == 0) {
                        if (VERBOSE) {
                            if (VERBOSE == 2) {
                                PRINTF("Couldn't erode foreground block with index %d.\r\n", i * 4 + j);
                            }
                        }
                    } else if (result == 1) {
                        if (VERBOSE) {
                            if (VERBOSE == 2) {
                                PRINTF("Eroded foreground block with index %d.\r\n", i * 4 + j);
                            }
                        }
                    }
                }
            }
        }
    }

    if (VERBOSE) {
        PRINTF("Finished erosion of lone segmentation blocks on iamge.\r\n");
    }

    return 1;
}

int PrintSegmentationArray(unsigned char *segmentation_array, int segmentation_array_size, unsigned int image_length, unsigned int image_width) {
    int i, j;
    int block_counter = 0;
    int no_blocks_rows = GetRowsOfBlocks(image_length);
    int no_blocks_cols = GetColsOfBlocks(image_width);

    PRINTF("Segmentation array:\r\n");

    for (i = 0; i < segmentation_array_size; ++i) {
        char no_blocks_saved_in_char = 4;

        if (i == segmentation_array_size - 1 && (no_blocks_rows * no_blocks_cols) % 4) {
            no_blocks_saved_in_char = (no_blocks_rows * no_blocks_cols) % 4;
        }

        for (j = 0; j < no_blocks_saved_in_char; ++j) {
            // Create mask at certain position in character to retrieve the
            // segmentation result of the block from the char
            char mask = BLOCK_MASK << (6 - 2 * j);

            printf("%d ", (mask & segmentation_array[i]) >> (6 - 2 * j));

            block_counter += 1;

            if (block_counter == no_blocks_cols) {
                PRINTF("\r\n");
                block_counter = 0;
            }
        }
    }

    PRINTF("\r\n");
}

int Segment(unsigned char image[MAX_IMAGE_LENGTH][MAX_IMAGE_WIDTH], unsigned int length, unsigned int width, unsigned char *segmentation_array, int *segmentation_array_size) {
    int i;
    int j;

    if (VERBOSE) {
        PRINTF("Starting segmentation of image.\r\n");
    }

    if (VERBOSE) {
        PRINTF("Starting calculation of block segmentation.\r\n");
    }

    int no_blocks_rows = GetRowsOfBlocks(length);
    int no_blocks_cols = GetColsOfBlocks(width);

    *segmentation_array_size = (no_blocks_rows * no_blocks_cols) / 4 + 1 * ((no_blocks_rows * no_blocks_cols) % 4 != 0);

    for (i = 0; i < *segmentation_array_size; ++i) {
        // Initialize segmented block to 0 before filling it with block
        // segmentation codes
        segmentation_array[i] = 0;

        char block_code = 0;
        char no_blocks_to_be_saved_in_char = 4;

        if (i == *segmentation_array_size - 1 && (no_blocks_rows * no_blocks_cols) % 4) {
            no_blocks_to_be_saved_in_char = (no_blocks_rows * no_blocks_cols) % 4;
        }

        for (j = 0; j < no_blocks_to_be_saved_in_char; ++j) {
            int block_start_row = 0;
            int block_start_col = 0;

            if (VERBOSE) {
                if (VERBOSE == 2) {
                    PRINTF("Block: %d\r\n", i * 4 + j);
                }
            }

            GetBlockStartFromIndex(i * 4 + j, length, width, &block_start_row, &block_start_col);

            float block_mean;
            float block_variance;

            block_mean = CalculateBlockMean(image, length, width, block_start_row, block_start_col);
            block_variance = CalculateBlockVariance(image, length, width, block_start_row, block_start_col, block_mean);

            if (VERBOSE) {
                if (VERBOSE == 2) {
                    PRINTF("Block mean: %d\nBlock variance: %d\r\n", (int) block_mean, (int) block_variance);
                }
            }

            // Determine type of block based if the variance of the block is
            // higher or lower than the threshold
            if (block_variance < BLOCK_VARIANCE_THRESHOLD) {
                block_code = BACKGROUND_BLOCK;
            } else {
                block_code = FOREGROUND_BLOCK;
            }

            // Create mask at certain position in character to save the
            // segmentation  result of the block into the char
            char mask = BLOCK_MASK << (6 - 2 * j);

            // Set the corresponding 2 bits of the character corresponding to
            // the position of the block
            segmentation_array[i] = (segmentation_array[i] & ~mask) | (block_code << (6 - 2 * j));
        }
    }

    if (VERBOSE) {
        PRINTF("Finished calculation of block segmentation.\r\n");
    }

    // Iterate over blocks and fill in those blocks for which most of
    // surrounding neighbor blocks are considered foreground blocks
    // and remove those blocks for which most of the surrounding neighbor blocks
    // are background blocks
    ErodeLoneBlocks(segmentation_array, *segmentation_array_size, no_blocks_rows, no_blocks_cols);

    if (VERBOSE) {
        PRINTF("Finished segmentation of image.\n");
    }

    return 1;
}
