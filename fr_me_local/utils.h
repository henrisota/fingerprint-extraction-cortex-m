#ifndef _UTILS_H
#define _UTILS_H

void print_char_binary(char c);

float calculate_image_mean(unsigned char **image, long length, long width);
float calculate_image_variance(unsigned char **image, float mean, long length, long width);
float calculate_block_mean(unsigned char **image, long length, long width, int block_start_row, int block_start_col);
float calculate_block_variance(unsigned char **image, long length, long width, int block_start_row, int block_start_col, float mean);

int get_rows_of_blocks(long length);
int get_cols_of_blocks(long width);

int get_block_start_from_index(int i, int length, int width, int *block_start_row, int *block_start_col);

int set_block(unsigned char **image, int block_index, long image_length, long image_width, unsigned char intensity);
int clean_block(unsigned char **image, int block_index, long image_length, long image_width);

#endif