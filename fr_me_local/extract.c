#include <stdio.h>

#include "config.h"
#include "imageio.h"
#include "orientation.h"
#include "segment.h"
#include "thin.h"
#include "utils.h"

#define RIDGE_ENDING      1
#define RIDGE_BIFURCATION 2

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

const int transitions_row_positions[9] = {P2_X, P3_X, P4_X, P5_X, P6_X, P7_X, P8_X, P9_X, P2_X};
const int transitions_col_positions[9] = {P2_Y, P3_Y, P4_Y, P5_Y, P6_Y, P7_Y, P8_Y, P9_Y, P2_Y};

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

int crossing_number(unsigned char **image, long width, int pixel_pos, int *neighbor_number) {
    int cn = 0;
    int neighbor_pixel_pos;
    int successive_neighbor_pixel_pos;
    unsigned char neighbor_pixel_intensity;
    unsigned char successive_neighbor_pixel_intensity;
    int d;

    // Calculate number of valley to ridge or vice versa transitions
    // between neighbors in clockwise fashion
    for (d = 0; d < 8; ++d) {
        neighbor_pixel_pos = pixel_pos + width * transitions_row_positions[d] + transitions_col_positions[d];
        successive_neighbor_pixel_pos = pixel_pos + width * transitions_row_positions[d + 1] + transitions_col_positions[d + 1];

        neighbor_pixel_intensity = image[neighbor_pixel_pos / MAX_IMAGE_WIDTH][neighbor_pixel_pos % MAX_IMAGE_WIDTH];
        successive_neighbor_pixel_intensity = image[successive_neighbor_pixel_pos / MAX_IMAGE_WIDTH][successive_neighbor_pixel_pos % MAX_IMAGE_WIDTH];

        if (neighbor_pixel_intensity == RIDGE_INTENSITY) {
            (*neighbor_number) += 1;
        }

        // Check for valley to ridge or vice versa transition
        if ((neighbor_pixel_intensity == VALLEY_INTENSITY && successive_neighbor_pixel_intensity == RIDGE_INTENSITY) ||
            (neighbor_pixel_intensity == RIDGE_INTENSITY && successive_neighbor_pixel_intensity == VALLEY_INTENSITY)) {
            ++cn;
        }
    }

    cn >>= 1;

    return cn;
}

int extract_minutiae(unsigned char **image, long length, long width, unsigned char *orientation_array, int orientation_array_size) {
    int i, j;
    int pixel_pos;
    int cn;
    int neighbor_number;
    int block_orientation;

    if (VERBOSE) {
        printf("Starting extraction using Crossing Number concept.\n");
    }

    int no_blocks_cols = get_cols_of_blocks(width);

    FILE *fp;

    fp = fopen("fingerprint_minutiae.csv", "w+");

    if (fp == NULL) {
        printf("Output file could not be created or opened and rewritten.\n");
        return 0;
    }

    for (i = 1; i < length - 1; ++i) {
        for (j = 1; j < width - 1; ++j) {
            pixel_pos = width * i + j;

            if (image[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] == RIDGE_INTENSITY) {
                neighbor_number = 0;
                cn = crossing_number(image, width, pixel_pos, &neighbor_number);

                block_orientation = orientation_array[(i / BLOCK_ROW_SIZE) * no_blocks_cols + (j / BLOCK_COL_SIZE)];

                if (neighbor_number == 1 && cn == 1) {
                    // printf("%03d,%03d,%03d,%d\n", i, j, block_orientation, RIDGE_ENDING);
                    fprintf(fp, "%03d,%03d,%03d,%d\n", i, j, block_orientation, RIDGE_ENDING);
                } else if (neighbor_number == 3 && cn == 3) {
                    // printf("%03d,%03d,%03d,%d\n", i, j, block_orientation, RIDGE_BIFURCATION);
                    fprintf(fp, "%03d,%03d,%03d,%d\n", i, j, block_orientation, RIDGE_BIFURCATION);
                }
            }
        }
    }

    fputc('\n', fp);

    fclose(fp);

    if (VERBOSE) {
        printf("Finished extraction using Crossing Number concept.\n");
    }

    return 1;
}

int extract(unsigned char **image, long length, long width, unsigned char **segmentation_array, int segmentation_array_size, unsigned char **orientation_array, int *orientation_array_size) {
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

    print_segmentation_array(*segmentation_array, segmentation_array_size, length, width);

    // apply_block_division_on_image(image, length, width, segmentation_array, segmentation_array_size);

    *orientation_array = calculate_local_orientation(image, length, width, segmentation_array, segmentation_array_size, orientation_array_size);

    // print_orientation_array(*orientation_array, *orientation_array_size, length, width);

    thin(image, length, width, segmentation_array, segmentation_array_size);

    extract_minutiae(image, length, width, *orientation_array, *orientation_array_size);

    if (VERBOSE) {
        printf("Finished extraction of features.\n");
    }

    return 1;
}