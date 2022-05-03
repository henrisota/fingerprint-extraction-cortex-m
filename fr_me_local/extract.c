#include <stdio.h>

#include "config.h"
#include "imageio.h"
#include "segment.h"
#include "thin.h"
#include "utils.h"

int divide_blocks_into_types(unsigned char **image, long image_length, long image_width, unsigned char **segmentation_array, int segmentation_array_size) {
    int i;
    int j;

    if (VERBOSE) {
        printf("Starting division of blocks into types.\n");
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

            // Divide foreground blocks
            if ((mask & (*segmentation_array)[i]) >> (6 - 2 * j) == 1) {
                if (VERBOSE) {
                    if (VERBOSE == 2) {
                        printf("Dividing block (%d | %d)\n", (i * 4 + j) / no_blocks_cols, (i * 4 + j) % no_blocks_cols);
                    }
                }

                int r;
                int c;
                int sum_of_rows[BLOCK_ROW_SIZE] = {0};
                int sum_of_cols[BLOCK_COL_SIZE] = {0};

                // Set the initial minimum to a value as the biggest possible (8 * 255)
                int min_sum_of_rows = 2040;
                int min_sum_of_cols = 2040;

                int curr_block_length = BLOCK_ROW_SIZE;
                int curr_block_width = BLOCK_COL_SIZE;

                int block_start_row = 0;
                int block_start_col = 0;

                get_block_start_from_index(i * 4 + j, image_length, image_width, &block_start_row, &block_start_col);

                if (image_length - block_start_row < BLOCK_ROW_SIZE) {
                    curr_block_length = image_length - block_start_row;
                }

                if (image_width - block_start_col < BLOCK_COL_SIZE) {
                    curr_block_width = image_width - block_start_col;
                }

                for (r = block_start_row; r < block_start_row + curr_block_length; ++r) {
                    for (c = block_start_col; c < block_start_col + curr_block_width; ++c) {
                        int pixel_pos = image_width * r + c;
                        unsigned char intensity = image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH];

                        sum_of_rows[r - block_start_row] += intensity;
                        sum_of_cols[c - block_start_col] += intensity;
                    }
                }

                for (r = 0; r < curr_block_length; ++r) {
                    min_sum_of_rows = (sum_of_rows[r] < min_sum_of_rows) ? sum_of_rows[r] : min_sum_of_rows;
                }

                for (c = 0; c < curr_block_width; ++c) {
                    min_sum_of_cols = (sum_of_cols[c] < min_sum_of_cols) ? sum_of_cols[c] : min_sum_of_cols;
                }

                if (min_sum_of_rows < min_sum_of_cols) {
                    // Mark block as type 1 (near horizontal ridge)
                    // Set the corresponding 2 bits of the character
                    // corresponding to the position of the block
                    (*segmentation_array)[i] = ((*segmentation_array)[i] & ~mask) | (TYPE_1_H_BLOCK << (6 - 2 * j));

                    if (VERBOSE) {
                        if (VERBOSE == 2) {
                            printf("Dividing block (%d | %d) as type 1 (H).\n", (i * 4 + j) / no_blocks_cols, (i * 4 + j) % no_blocks_cols);
                        }
                    }
                } else {
                    // Mark block as type 2 (near vertical ridge)
                    // Set the corresponding 2 bits of the character
                    // corresponding to the position of the block
                    (*segmentation_array)[i] = ((*segmentation_array)[i] & ~mask) | (TYPE_2_V_BLOCK << (6 - 2 * j));

                    if (VERBOSE) {
                        if (VERBOSE == 2) {
                            printf("Dividing block (%d | %d) as type 2 (V).\n", (i * 4 + j) / no_blocks_cols, (i * 4 + j) % no_blocks_cols);
                        }
                    }
                }
            }
        }
    }

    if (VERBOSE) {
        printf("Finished division of blocks into types.\n");
    }

    return 1;
}

int erode_lone_type_blocks(char block_type, unsigned char **segmentation_array, int segmentation_array_size, int no_blocks_rows, int no_blocks_cols) {
    char mask;
    int i;
    int j;
    int k;
    int l;

    int no_blocks = no_blocks_rows * no_blocks_cols;

    int index = 0;
    int neighbor_index = 0;

    int count_neighbors = 0;
    int count_neighbor_type_1_blocks = 0;
    int count_neighbor_type_2_blocks = 0;

    if (VERBOSE) {
        printf("Starting erosion of type %d blocks.\n", block_type);
    }

    // Iterate over each block
    for (i = 0; i < segmentation_array_size; ++i) {
        char no_blocks_saved_in_char = 4;

        if (i == segmentation_array_size - 1 && (no_blocks_rows * no_blocks_cols) % 4) {
            no_blocks_saved_in_char = (no_blocks_rows * no_blocks_cols) % 4;
        }

        for (j = 0; j < no_blocks_saved_in_char; ++j) {
            // Create mask at certain position in character to retrieve the
            // segmentation result of the block from the char
            mask = BLOCK_MASK << (6 - 2 * j);

            // Perform erosion on given block types
            if ((mask & (*segmentation_array)[i]) >> (6 - 2 * j) == block_type) {
                if (VERBOSE) {
                    if (VERBOSE == 2) {
                        printf("Eroding block (%d | %d)\n", (i * 4 + j) / no_blocks_cols, (i * 4 + j) % no_blocks_cols);
                    }
                }

                count_neighbors = 0;
                count_neighbor_type_1_blocks = 0;
                count_neighbor_type_2_blocks = 0;
                index = i * 4 + j;

                // Iterate over neighboring blocks and check their type
                for (k = -1; k <= 1; ++k) {
                    for (l = -1; l <= 1; ++l) {
                        if (k != 0 || l != 0) {
                            neighbor_index = index + k * no_blocks_cols + l;

                            if (neighbor_index >= 0 && neighbor_index < no_blocks) {
                                // Create mask at certain position in character to retrieve the
                                // segmentation result of the block from the char
                                mask = BLOCK_MASK << (6 - 2 * (neighbor_index % 4));

                                char neighbor_block_code = (mask & (*segmentation_array)[neighbor_index / 4]) >> (6 - 2 * (neighbor_index % 4));

                                if (neighbor_block_code == TYPE_1_H_BLOCK) {
                                    ++count_neighbors;

                                    if (VERBOSE) {
                                        if (VERBOSE == 2) {
                                            printf("Neighbor with index %d (%d | %d) is type 1 (H)\n", neighbor_index, neighbor_index / no_blocks_cols, neighbor_index % no_blocks_cols);
                                        }
                                    }

                                    count_neighbor_type_1_blocks += 1;
                                } else if (neighbor_block_code == TYPE_2_V_BLOCK) {
                                    ++count_neighbors;

                                    if (VERBOSE) {
                                        if (VERBOSE == 2) {
                                            printf("Neighbor with index %d (%d | %d) is type 2 (V)\n", neighbor_index, neighbor_index / no_blocks_cols, neighbor_index % no_blocks_cols);
                                        }
                                    }

                                    count_neighbor_type_2_blocks += 1;
                                }
                            }
                        }
                    }
                }

                if (VERBOSE) {
                    if (VERBOSE == 2) {
                        printf("Number of neighboring type 1 blocks: %d\n", count_neighbor_type_1_blocks);
                        printf("Number of neighboring type 2 blocks: %d\n", count_neighbor_type_2_blocks);
                    }
                }

                // Calculate mask to set the corresponding block on the character
                mask = BLOCK_MASK << (6 - 2 * (index % 4));

                // If the block is of type 1 mark the block as type 2 block if the number
                // of type 2 neighbors is higher than the threshold or the number of
                // neighbor type 1 blocks. Otherwise, if the block is of type 2 and the
                // number of type 1 neighbors is higher than the threshold or the number of
                // neighbor type 2 blocks, mark the block as type 1
                if (block_type == TYPE_1_H_BLOCK && (count_neighbor_type_2_blocks > TYPE_1_BLOCK_EROSION_TYPE_2_NEIGHBORS_THRESHOLD || count_neighbor_type_2_blocks > count_neighbor_type_1_blocks)) {
                    // Set the corresponding 2 bits of the character corresponding to
                    // the position of the block
                    (*segmentation_array)[index / 4] = ((*segmentation_array)[index / 4] & ~mask) | (TYPE_2_V_BLOCK << (6 - 2 * (index % 4)));
                } else if (block_type == TYPE_2_V_BLOCK && (count_neighbor_type_1_blocks > TYPE_2_BLOCK_EROSION_TYPE_1_NEIGHBORS_THRESHOLD || count_neighbor_type_1_blocks > count_neighbor_type_2_blocks)) {
                    // Set the corresponding 2 bits of the character corresponding to
                    // the position of the block
                    (*segmentation_array)[index / 4] = ((*segmentation_array)[index / 4] & ~mask) | (TYPE_1_H_BLOCK << (6 - 2 * (index % 4)));
                }
            }
        }
    }

    return 1;
}

int apply_block_division_on_image(unsigned char **image, long image_length, long image_width, unsigned char **segmentation_array, int segmentation_array_size) {
    int i;
    int j;

    if (VERBOSE) {
        printf("Starting applying block division on image.\n");
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
            char block_code = (mask & (*segmentation_array)[i]) >> (6 - 2 * j);

            if (block_code == TYPE_1_H_BLOCK) {
                set_block(image, i * 4 + j, image_length, image_width, TYPE_1_BLOCK_OUTPUT_INTENSITY);
            } else if (block_code == TYPE_2_V_BLOCK) {
                set_block(image, i * 4 + j, image_length, image_width, TYPE_2_BLOCK_OUTPUT_INTENSITY);
            }
        }
    }

    if (VERBOSE) {
        printf("Finished applying block division on image.\n");
    }

    return 1;
}

int extract(unsigned char **image, long length, long width, unsigned char **segmentation_array, int segmentation_array_size) {
    if (VERBOSE) {
        printf("Starting extraction of features\n");
    }

    int no_blocks_rows = get_rows_of_blocks(length);
    int no_blocks_cols = get_cols_of_blocks(width);

    divide_blocks_into_types(image, length, width, segmentation_array, segmentation_array_size);

    // Perform erosion on type 1 (horizontal) blocks which are found between type 2 (vertical) blocks
    erode_lone_type_blocks(TYPE_1_H_BLOCK, segmentation_array, segmentation_array_size, no_blocks_rows, no_blocks_cols);

    // Perform erosion on type 2 (vertical) blocks which are found between type 1 (horizontal) blocks
    erode_lone_type_blocks(TYPE_2_V_BLOCK, segmentation_array, segmentation_array_size, no_blocks_rows, no_blocks_cols);

    // apply_block_division_on_image(image, length, width, segmentation_array, segmentation_array_size);

    thin(image, length, width, segmentation_array, segmentation_array_size);

    print_segmentation_array(*segmentation_array, segmentation_array_size, length, width);

    print_image_array(image, length, width, OUTPUT_TO_FILE);

    if (VERBOSE) {
        printf("Finished extraction of features.\n");
    }

    return 1;
}