#include "imageio.h"

int extract_short_from_buffer(char buffer[], int lsb, int start, short *number) {
    union short_char_union lcu;

    lcu.s_num = 0;

    if (lsb == 1) {
        lcu.s_alpha[0] = buffer[start + 0];
        lcu.s_alpha[1] = buffer[start + 1];
    }

    if (lsb == 0) {
        lcu.s_alpha[0] = buffer[start + 1];
        lcu.s_alpha[1] = buffer[start + 0];
    }

    *number = lcu.s_num;

    return 1;
}

int extract_ushort_from_buffer(char buffer[], int lsb, int start, unsigned short *number) {
    union ushort_char_union lcu;

    lcu.s_num = 0;

    if (lsb == 1) {
        lcu.s_alpha[0] = buffer[start + 0];
        lcu.s_alpha[1] = buffer[start + 1];
    }

    if (lsb == 0) {
        lcu.s_alpha[0] = buffer[start + 1];
        lcu.s_alpha[1] = buffer[start + 0];
    }

    *number = lcu.s_num;

    return 1;
}

int extract_long_from_buffer(char buffer[], int lsb, int start, long *number) {
    union long_char_union lcu;

    lcu.l_num = 0;

    if (lsb == 1) {
        lcu.l_alpha[0] = buffer[start + 0];
        lcu.l_alpha[1] = buffer[start + 1];
        lcu.l_alpha[2] = buffer[start + 2];
        lcu.l_alpha[3] = buffer[start + 3];
    }

    if (lsb == 0) {
        lcu.l_alpha[0] = buffer[start + 3];
        lcu.l_alpha[1] = buffer[start + 2];
        lcu.l_alpha[2] = buffer[start + 1];
        lcu.l_alpha[3] = buffer[start + 0];
    }

    *number = lcu.l_num;

    return 1;
}

int extract_ulong_from_buffer(char buffer[], int lsb, int start, unsigned long *number) {
    union ulong_char_union lcu;

    lcu.l_num = 0;

    if (lsb == 1) {
        lcu.l_alpha[0] = buffer[start + 0];
        lcu.l_alpha[1] = buffer[start + 1];
        lcu.l_alpha[2] = buffer[start + 2];
        lcu.l_alpha[3] = buffer[start + 3];
    }

    if (lsb == 0) {
        lcu.l_alpha[0] = buffer[start + 3];
        lcu.l_alpha[1] = buffer[start + 2];
        lcu.l_alpha[2] = buffer[start + 1];
        lcu.l_alpha[3] = buffer[start + 0];
    }

    *number = lcu.l_num;

    return 1;
}

int calculate_pad(long width) {
    int pad = 0;

    pad = ((width % 4) == 0) ? 0 : (4 - (width % 4));

    return pad;
}

int is_a_bmp(char *file_name) {
    char *cc;
    int result = 0;
    struct bmpfileheader file_header;

    cc = strstr(file_name, ".bmp");

    if (cc == NULL) {
        return result;
    }

    read_bmp_file_header(file_name, &file_header);

    // Filetype for .bmp files is "BM"  encoded in the first 2 bytes as (0x4d42)
    if (file_header.filetype != 0x4d42) {
        return result;
    }

    result = 1;

    return result;
}

int read_bmp_file_header(char *file_name, struct bmpfileheader *file_header) {
    // Hold temporary data retrieved from image
    char buffer[10];
    short ss;
    unsigned long ull;
    unsigned short uss;

    FILE *fp;

    // File header for .bmp files is 14B
    fp = fopen(file_name, "rb");

    fread(buffer, 1, BITMAP_FILE_HEADER_SIGNATURE_SIZE, fp);
    extract_ushort_from_buffer(buffer, 1, 0, &uss);
    file_header->filetype = uss;

    fread(buffer, 1, BITMAP_FILE_HEADER_FILE_SIZE_SIZE, fp);
    extract_ulong_from_buffer(buffer, 1, 0, &ull);
    file_header->filesize = ull;

    fread(buffer, 1, BITMAP_FILE_HEADER_RESERVED_ONE_SIZE, fp);
    extract_short_from_buffer(buffer, 1, 0, &ss);
    file_header->reserved1 = ss;

    fread(buffer, 1, BITMAP_FILE_HEADER_RESERVED_TWO_SIZE, fp);
    extract_short_from_buffer(buffer, 1, 0, &ss);
    file_header->reserved2 = ss;

    fread(buffer, 1, BITMAP_FILE_HEADER_DATA_OFFSET_SIZE, fp);
    extract_ulong_from_buffer(buffer, 1, 0, &ull);
    file_header->bitmapoffset = ull;

    fclose(fp);

    return 1;
}

void print_bmp_file_header(struct bmpfileheader *file_header) {
    printf("File type: %x\n", file_header->filetype);
    printf("File size: %lu\n", file_header->filesize);
    printf("Bit map offset: %lu\n", file_header->bitmapoffset);
}

int read_bm_header(char *file_name, struct bitmapheader *bm_header) {
    // Hold temporary data retrieved from image
    char buffer[10];
    long ll;
    unsigned long ull;
    unsigned short uss;

    FILE *fp;

    fp = fopen(file_name, "rb");

    // Move past the file header at the beginning
    fseek(fp, BITMAP_INFORMATION_HEADER_OFFSET, SEEK_SET);

    fread(buffer, 1, BITMAP_INFORMATION_HEADER_SIZE_SIZE, fp);
    extract_ulong_from_buffer(buffer, 1, 0, &ull);
    bm_header->size = ull;

    fread(buffer, 1, BITMAP_INFORMATION_HEADER_WIDTH_SIZE, fp);
    extract_long_from_buffer(buffer, 1, 0, &ll);
    bm_header->width = ll;

    fread(buffer, 1, BITMAP_INFORMATION_HEADER_HEIGHT_SIZE, fp);
    extract_long_from_buffer(buffer, 1, 0, &ll);
    bm_header->height = ll;

    fread(buffer, 1, BITMAP_INFORMATION_HEADER_COLOR_PLANES_SIZE, fp);
    extract_ushort_from_buffer(buffer, 1, 0, &uss);
    bm_header->planes = uss;

    fread(buffer, 1, BITMAP_INFORMATION_HEADER_BITS_PER_PIXEL_SIZE, fp);
    extract_ushort_from_buffer(buffer, 1, 0, &uss);
    bm_header->bitsperpixel = uss;

    fread(buffer, 1, BITMAP_INFORMATION_HEADER_COMPRESSION_SIZE, fp);
    extract_ulong_from_buffer(buffer, 1, 0, &ull);
    bm_header->compression = ull;

    fread(buffer, 1, BITMAP_INFORMATION_HEADER_IMAGE_SIZE_SIZE, fp);
    extract_ulong_from_buffer(buffer, 1, 0, &ull);
    bm_header->sizeofbitmap = ull;

    fread(buffer, 1, BITMAP_INFORMATION_HEADER_HORIZONTAL_RESOLUTION_SIZE, fp);
    extract_ulong_from_buffer(buffer, 1, 0, &ull);
    bm_header->horzres = ull;

    fread(buffer, 1, BITMAP_INFORMATION_HEADER_VERTICAL_RESOLUTION_SIZE, fp);
    extract_ulong_from_buffer(buffer, 1, 0, &ull);
    bm_header->vertres = ull;

    fread(buffer, 1, BITMAP_INFORMATION_HEADER_NUMBER_OF_COLORS_USED_SIZE, fp);
    extract_ulong_from_buffer(buffer, 1, 0, &ull);
    bm_header->colorsused = ull;

    fread(buffer, 1, BITMAP_INFORMATION_HEADER_NUMBER_OF_IMPORTANT_COLORS_USED_SIZE, fp);
    extract_ulong_from_buffer(buffer, 1, 0, &ull);
    bm_header->colorsimp = ull;

    fclose(fp);

    return 1;
}

void print_bm_header(struct bitmapheader *bm_header) {
    printf("Width: %ld\n", bm_header->width);
    printf("Height: %ld\n", bm_header->height);
    printf("Color planes: %d\n", bm_header->planes);
    printf("Bits per pixel: %d\n", bm_header->bitsperpixel);
    printf("Colors used: %lu\n", bm_header->colorsused);
    printf("Important colors: %lu\n", bm_header->colorsimp);
}

int read_color_table(char *file_name, struct ctstruct *rgb, int size) {
    int i;
    char buffer[10];

    FILE *fp;

    fp = fopen(file_name, "rb");

    // Move past the file header and the information header
    fseek(fp, BITMAP_COLOR_TABLE_OFFSET, SEEK_SET);

    for (i = 0; i < size; ++i) {
        fread(buffer, 1, 1, fp);
        rgb[i].blue = buffer[0];

        fread(buffer, 1, 1, fp);
        rgb[i].green = buffer[0];

        fread(buffer, 1, 1, fp);
        rgb[i].red = buffer[0];

        // Fourth byte is used as padding and is not saved
        fread(buffer, 1, 1, fp);
    }

    fclose(fp);

    return 1;
}

void print_color_table(struct ctstruct *rgb, int size) {
    int i;

    for (i = 0; i < size; ++i) {
        printf("B: %d G: %d R: %d\n", rgb[i].blue, rgb[i].green, rgb[i].red);
    }
}

int read_bmp_image(char *file_name, short **array) {
    FILE *fp;

    int i;
    int j;

    // Flag for keeping track if height is negative indicating bottom up read
    int negative = 0;

    // int pad = 0;
    int place = 0;

    long width = 0;
    long height = 0;

    // Keep track of the number of colors used in the image
    long colors = 0;

    struct bmpfileheader file_header;
    struct bitmapheader bm_header;
    struct ctstruct rgb[GRAY_LEVELS + 1];

    unsigned char uc;

    read_bmp_file_header(file_name, &file_header);
    read_bm_header(file_name, &bm_header);

    if (bm_header.compression != NO_COMPRESION) {
        printf("Cannot read compressed images\n");
        exit(1);
    }

    if (bm_header.bitsperpixel != 8) {
        printf("Cannot read images with bits per pixel different than 8\n");
        exit(1);
    }

    if (bm_header.colorsused == 0) {
        colors = GRAY_LEVELS + 1;
    } else {
        colors = bm_header.colorsused;
    }

    read_color_table(file_name, rgb, colors);

    fp = fopen(file_name, "rb");

    fseek(fp, file_header.bitmapoffset, SEEK_SET);

    width = bm_header.width;

    if (bm_header.height < 0) {
        height = -bm_header.height;
        negative = 1;
    } else {
        height = bm_header.height;
    }

    // pad = calculate_pad(width);

    for (i = 0; i < height; ++i) {
        for (j = 0; j < width; ++j) {
            place = fgetc(fp);
            uc = (place & 0xff);
            place = uc;

            // Grayscale image have all three components set to same value
            array[i][j] = rgb[place].blue;
        }

        // if (pad != 0) {
        //     position = fseek(fp, pad, SEEK_CUR);
        // }
    }

    if (negative == 0) {
        flip_image_array(array, height, width);
    }

    return 1;
}

short **allocate_image_array(long length, long width) {
    int i;
    int j;

    short **image_array = malloc(length * sizeof(short *));

    if (image_array == NULL) {
        printf("malloc of image_array failed\n");
        exit(1);
    }

    for (i = 0; i < length; ++i) {
        image_array[i] = malloc(width * sizeof(short));

        if (image_array[i] == NULL) {
            printf("malloc of image_array[%d] failed\n", i);
            exit(1);
        }
    }

    for (i = 0; i < length; ++i) {
        for (j = 0; j < width; ++j) {
            image_array[i][j] = 0;
        }
    }

    return image_array;
}

int free_image_array(short **image_array, long length) {
    int i;

    for (i = 0; i < length; ++i)
        free(image_array[i]);

    free(image_array);

    return 1;
}

int read_image_array(char *file_name, short **image_array) {
    int success = 0;

    if (is_a_bmp(file_name)) {
        read_bmp_image(file_name, image_array);

        success = 1;
    }

    if (success == 0) {
        printf("Could not read file\n");
        exit(1);
    }

    return 1;
}

void print_image_array(short **image_array, long length, long width) {
    int i;
    int j;

    for (i = 0; i < length; ++i) {
        for (j = 0; j < width; ++j) {
            if (j) {
                printf(",");
            }
            printf("%d", image_array[i][j]);
        }

        printf("\n");
    }
}

int create_grayscale_binary_file(short **image_array, long length, long width) {
    FILE *fp;
    int i;
    int j;

    fp = fopen("binary_image.gbf", "w+");

    if (fp == NULL) {
        printf("Output file could not be created or opened and rewritten.\n");
        return 0;
    }

    fputc(((unsigned long) length & 0xff00) >> 8, fp);
    fputc(((unsigned long) length & 0x00ff), fp);
    fputc(((unsigned long) width & 0xff00) >> 8, fp);
    fputc(((unsigned long) width & 0x00ff), fp);

    for (i = 0; i < length; ++i) {
        for (j = 0; j < width; ++j) {
            fputc(image_array[i][j], fp);
        }
    }

    fclose(fp);

    return 1;
}

int flip_image_array(short **image_array, long rows, long cols) {
    int i;
    int j;
    long rd2;
    short **temp;

    temp = allocate_image_array(rows, cols);

    rd2 = rows / 2;

    for (i = 0; i < rd2; ++i) {
        for (int j = 0; j < cols; ++j) {
            temp[rows - 1 - i][j] = image_array[i][j];
        }
    }

    for (i = rd2; i < rows; ++i) {
        for (j = 0; j < cols; ++j) {
            temp[rows - 1 - i][j] = image_array[i][j];
        }
    }

    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j) {
            image_array[i][j] = temp[i][j];
        }
    }

    free_image_array(temp, rows);

    return 1;
}

int get_image_size(char *file_name, long *rows, long *cols) {
    int is_bmp = 0;
    int result = 0;

    struct bitmapheader bmph;

    if (is_a_bmp(file_name)) {
        is_bmp = 1;

        read_bm_header(file_name, &bmph);

        *rows = bmph.height;
        *cols = bmph.width;
    }

    if (is_bmp == 1) {
        result = 1;
    }

    return result;
}

int get_bits_per_pixel(char *file_name, long *bitsperpixel) {
    int is_bmp = 0;
    int result = 0;

    struct bitmapheader bmph;

    if (is_a_bmp(file_name)) {
        is_bmp = 1;

        read_bm_header(file_name, &bmph);

        *bitsperpixel = bmph.bitsperpixel;
    }

    if (is_bmp == 1) {
        result = 1;
    }

    return result;
}
