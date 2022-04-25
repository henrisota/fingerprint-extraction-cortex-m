#ifndef _IMAGEIO_H
#define _IMAGEIO_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Bitmap Static Header Sizes
#define BITMAP_FILE_HEADER_SIZE       14
#define BITMAP_INFORMATON_HEADER_SIZE 40

// Bitmap File Header Field Sizes and Offsets
#define BITMAP_FILE_HEADER_OFFSET              0x0000
#define BITMAP_FILE_HEADER_SIGNATURE_SIZE      2
#define BITMAP_FILE_HEADER_SIGNATURE_OFFSET    0x0000
#define BITMAP_FILE_HEADER_FILE_SIZE_SIZE      4
#define BITMAP_FILE_HEADER_FILE_SIZE_OFFSET    0x0002
#define BITMAP_FILE_HEADER_RESERVED_ONE_SIZE   2
#define BITMAP_FILE_HEADER_RESERVED_ONE_OFFSET 0x0006
#define BITMAP_FILE_HEADER_RESERVED_TWO_SIZE   2
#define BITMAP_FILE_HEADER_RESERVED_TWO_OFFSET 0x0008
#define BITMAP_FILE_HEADER_DATA_OFFSET_SIZE    4
#define BITMAP_FILE_HEADER_DATA_OFFSET_OFFSET  0x000A

// Bitmap Information Header Sizes and Offsets
#define BITMAP_INFORMATION_HEADER_OFFSET                                 0x000E
#define BITMAP_INFORMATION_HEADER_SIZE_SIZE                              4
#define BITMAP_INFORMATION_HEADER_SIZE_OFFSET                            0x000E
#define BITMAP_INFORMATION_HEADER_WIDTH_SIZE                             4
#define BITMAP_INFORMATION_HEADER_WIDTH_OFFSET                           0x0012
#define BITMAP_INFORMATION_HEADER_HEIGHT_SIZE                            4
#define BITMAP_INFORMATION_HEADER_HEIGHT_OFFSET                          0x0016
#define BITMAP_INFORMATION_HEADER_COLOR_PLANES_SIZE                      2
#define BITMAP_INFORMATION_HEADER_COLOR_PLANES_OFFSET                    0x001A
#define BITMAP_INFORMATION_HEADER_BITS_PER_PIXEL_SIZE                    2
#define BITMAP_INFORMATION_HEADER_BITS_PER_PIXEL_OFFSET                  0x001C
#define BITMAP_INFORMATION_HEADER_COMPRESSION_SIZE                       4
#define BITMAP_INFORMATION_HEADER_COMPRESSION_OFFSET                     0x001E
#define BITMAP_INFORMATION_HEADER_IMAGE_SIZE_SIZE                        4
#define BITMAP_INFORMATION_HEADER_IMAGE_SIZE_OFFSET                      0x0022
#define BITMAP_INFORMATION_HEADER_HORIZONTAL_RESOLUTION_SIZE             4
#define BITMAP_INFORMATION_HEADER_HORIZONTAL_RESOLUTION_OFFSET           0x0026
#define BITMAP_INFORMATION_HEADER_VERTICAL_RESOLUTION_SIZE               4
#define BITMAP_INFORMATION_HEADER_VERTICAL_RESOLUTION_OFFSET             0x002A
#define BITMAP_INFORMATION_HEADER_NUMBER_OF_COLORS_USED_SIZE             4
#define BITMAP_INFORMATION_HEADER_NUMBER_OF_COLORS_USED_OFFSET           0x002E
#define BITMAP_INFORMATION_HEADER_NUMBER_OF_IMPORTANT_COLORS_USED_SIZE   4
#define BITMAP_INFORMATION_HEADER_NUMBER_OF_IMPORTANT_COLORS_USED_OFFSET 0x0032

// Bitmap Color Table Offset
#define BITMAP_COLOR_TABLE_OFFSET 0x0036

// Value of compression field when no compression is used on the image
#define NO_COMPRESION 0

// Number of gray levels
#define GRAY_LEVELS 0xFF

struct bmpfileheader {
    unsigned short filetype;
    unsigned long filesize;
    short reserved1;
    short reserved2;
    unsigned long bitmapoffset;
};

struct bitmapheader {
    unsigned long size;
    long width;
    long height;
    unsigned short planes;
    unsigned short bitsperpixel;
    unsigned long compression;
    unsigned long sizeofbitmap;
    unsigned long horzres;
    unsigned long vertres;
    unsigned long colorsused;
    unsigned long colorsimp;
};

struct ctstruct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

union short_char_union {
    short s_num;
    char s_alpha[2];
};

union int_char_union {
    int i_num;
    char i_alpha[4];
};

union long_char_union {
    long l_num;
    char l_alpha[8];
};

union float_char_union {
    float f_num;
    char f_alpha[4];
};

union ushort_char_union {
    short s_num;
    char s_alpha[2];
};

union uint_char_union {
    int i_num;
    char i_alpha[4];
};

union ulong_char_union {
    long l_num;
    char l_alpha[8];
};

// Function Definitions
int extract_short_from_buffer(char buffer[], int lsb, int start, short *number);
int extract_ushort_from_buffer(char buffer[], int lsb, int start, unsigned short *number);
int extract_long_from_buffer(char buffer[], int lsb, int start, long *number);
int extract_ulong_from_buffer(char buffer[], int lsb, int start, unsigned long *number);

int calculate_pad(long width);
int is_a_bmp(char *file_name);
int read_bmp_file_header(char *file_name, struct bmpfileheader *file_header);
int print_bmp_file_header(struct bmpfileheader *file_header);
int read_bm_header(char *file_name, struct bitmapheader *bm_header);
int print_bm_header(struct bitmapheader *bm_header);
int read_color_table(char *file_name, struct ctstruct *rgb, int size);
int print_color_table(struct ctstruct *rgb, int size);
int read_bmp_image(char *file_name, short **array);
short **allocate_image_array(long length, long width);
int free_image_array(short **image_array, long length);
int read_image_array(char *file_name, short **image_array);
int print_image_array(short **image_array, long length, long width);
int create_grayscale_binary_file(short **image_array, long length, long width);
int flip_image_array(short **image_array, long rows, long cols);
int get_image_size(char *file_name, long *rows, long *cols);
int get_bits_per_pixel(char *file_name, long *bitsperpixel);

#endif