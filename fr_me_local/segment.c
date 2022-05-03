#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "imageio.h"
#include "segment.h"
#include "utils.h"

int calculate_segmentation_array_size(int no_blocks_rows, int no_blocks_cols) {
    return (no_blocks_rows * no_blocks_cols) / 4 + 1 * ((no_blocks_rows * no_blocks_cols) % 4 != 0);
}

unsigned char *create_segmentation_array(int no_blocks_rows, int no_blocks_cols, int *segmentation_array_size) {
    *segmentation_array_size = calculate_segmentation_array_size(no_blocks_rows, no_blocks_cols);

    unsigned char *segmentation_array = malloc(*segmentation_array_size * sizeof(unsigned char));

    if (segmentation_array == NULL) {
        printf("malloc of segmentation_array failed\n");
        exit(1);
    }

    return segmentation_array;
}

int erode_block_from_index(char block_type, unsigned char *segmentation_array, int index, int no_blocks_rows, int no_blocks_cols) {
    int result = 0;
    char mask;
    int i;
    int j;
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
                                printf("Neighbor with index %d (%d | %d) is foreground\n", neighbor_index, neighbor_index / no_blocks_cols, neighbor_index % no_blocks_cols);
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
            printf("Number of neighboring foreground blocks: %d\n", count_neighbor_foreground_blocks);
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
        // segmentation_array[i] |= (0b01 << (6 - 2 * (index % 4)));

        result = 1;
    } else if (block_type == FOREGROUND_BLOCK && count_neighbor_foreground_blocks < FOREGROUND_EROSION_BACKGROUND_NEIGHBORS_THRESHOLD) {
        // Set the corresponding 2 bits of the character corresponding to
        // the position of the block
        segmentation_array[index / 4] = (segmentation_array[index / 4] & ~mask) | (BACKGROUND_BLOCK << (6 - 2 * (index % 4)));

        result = 1;
    }

    return result;
}

int erode_lone_blocks(unsigned char *segmentation_array, int segmentation_array_size, int no_blocks_rows, int no_blocks_cols) {
    int iteration = EROSION_ITERATIONS;
    int i;
    int j;
    int result = 0;

    if (VERBOSE) {
        printf("Starting erosion of lone segmentation blocks on image.\n");
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
                        printf("Eroding at block (%d | %d)\n", (i * 4 + j) / no_blocks_cols, (i * 4 + j) % no_blocks_cols);
                    }
                }

                if ((mask & segmentation_array[i]) >> (6 - 2 * j) == BACKGROUND_BLOCK) {
                    // Apply erosion on the background blocks
                    result = erode_block_from_index(BACKGROUND_BLOCK, segmentation_array, i * 4 + j, no_blocks_rows, no_blocks_cols);

                    if (result == 0) {
                        if (VERBOSE) {
                            if (VERBOSE == 2) {
                                printf("Couldn't erode background block with index %d.\n", i * 4 + j);
                            }
                        }
                    } else if (result == 1) {
                        if (VERBOSE) {
                            if (VERBOSE == 2) {
                                printf("Eroded background block with index %d.\n", i * 4 + j);
                            }
                        }
                    }
                } else {
                    // Apply erosion on the foreground blocks
                    result = erode_block_from_index(FOREGROUND_BLOCK, segmentation_array, i * 4 + j, no_blocks_rows, no_blocks_cols);

                    if (result == 0) {
                        if (VERBOSE) {
                            if (VERBOSE == 2) {
                                printf("Couldn't erode foreground block with index %d.\n", i * 4 + j);
                            }
                        }
                    } else if (result == 1) {
                        if (VERBOSE) {
                            if (VERBOSE == 2) {
                                printf("Eroded foreground block with index %d.\n", i * 4 + j);
                            }
                        }
                    }
                }
            }
        }
    }

    if (VERBOSE) {
        printf("Finished erosion of lone segmentation blocks on image.\n");
    }

    return 1;
}

int print_segmentation_array(unsigned char *segmentation_array, int segmentation_array_size, int image_length, int image_width) {
    int i;
    int j;
    int block_counter = 0;
    int no_blocks_rows = get_rows_of_blocks(image_length);
    int no_blocks_cols = get_cols_of_blocks(image_width);

    printf("Segmentation array:\n");

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
                printf("\n");
                block_counter = 0;
            }
        }
    }

    printf("\n");

    return 1;
}

int apply_segmentation_on_image(unsigned char **image, long image_length, long image_width, unsigned char **segmentation_array, int segmentation_array_size) {
    int i;
    int j;

    if (VERBOSE) {
        printf("Starting applying segmentation on image.\n");
    }

    int no_blocks_rows = get_rows_of_blocks(image_length);
    int no_blocks_cols = get_cols_of_blocks(image_width);

    for (i = 0; i < segmentation_array_size; ++i) {
        char no_blocks_saved_in_char = 4;

        if (i == segmentation_array_size - 1 && (no_blocks_rows * no_blocks_cols) % 4) {
            no_blocks_saved_in_char = (no_blocks_rows * no_blocks_cols) % 4;
        }

        for (j = 0; j < no_blocks_saved_in_char; ++j) {
            // Create mask at certain position in character to retrieve the
            // segmentation result of the block from the char
            char mask = BLOCK_MASK << (6 - 2 * j);

            if ((mask & (*segmentation_array)[i]) >> (6 - 2 * j) == BACKGROUND_BLOCK) {
                clean_block(image, i * 4 + j, image_length, image_width);
            }
        }
    }

    if (VERBOSE) {
        printf("Finished applying segmentation on image.\n");
    }

    return 1;
}

unsigned char *segment(unsigned char **image, long length, long width, int *segmentation_array_size) {
    int i;
    int j;

    if (VERBOSE) {
        printf("Starting segmentation of image.\n");
    }

    if (VERBOSE) {
        printf("Starting calculation of block segmentation.\n");
    }

    int no_blocks_rows = get_rows_of_blocks(length);
    int no_blocks_cols = get_cols_of_blocks(width);
    unsigned char *segmentation_array = create_segmentation_array(no_blocks_rows, no_blocks_cols, segmentation_array_size);

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
                    printf("Block: %d\n", i * 4 + j);
                }
            }

            get_block_start_from_index(i * 4 + j, length, width, &block_start_row, &block_start_col);

            float block_mean;
            float block_variance;

            block_mean = calculate_block_mean(image, length, width, block_start_row, block_start_col);
            block_variance = calculate_block_variance(image, length, width, block_start_row, block_start_col, block_mean);

            if (VERBOSE) {
                if (VERBOSE == 2) {
                    printf("Block mean: %f\nBlock variance: %f\n", block_mean, block_variance);
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
        printf("Finished calculation of block segmentation.\n");
    }

    // Iterate over blocks and fill in those blocks for which most of
    // surrounding neighbor blocks are considered foreground blocks
    // and remove those blocks for which most of the surrounding neighbor blocks
    // are background blocks
    erode_lone_blocks(segmentation_array, *segmentation_array_size, no_blocks_rows, no_blocks_cols);

    apply_segmentation_on_image(image, length, width, &segmentation_array, *segmentation_array_size);

    if (VERBOSE) {
        printf("Finished segmentation of image.\n");
    }

    return segmentation_array;
}