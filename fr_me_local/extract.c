#include <stdio.h>
#include <stdlib.h>

unsigned char **read_grayscale_binary_file(const char *filename, long *length, long *width) {
    FILE *fp;
    int i;
    int j;

    fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Output file could not be created or opened and rewritten.\n");
        return 0;
    }

    *length &= 0x0;

    *length = fgetc(fp);
    (*length) <<= 8;
    *length ^= fgetc(fp);

    *width &= 0x0;

    *width = fgetc(fp);
    (*width) <<= 8;
    *width ^= fgetc(fp);

    unsigned char **image_array = malloc(*length * sizeof(unsigned char *));

    if (image_array == NULL) {
        printf("malloc of image_array failed\n");
        exit(1);
    }

    for (i = 0; i < *length; ++i) {
        image_array[i] = malloc(*width * sizeof(short));

        if (image_array[i] == NULL) {
            printf("malloc of image_array[%d] failed\n", i);
            exit(1);
        }
    }

    for (i = 0; i < *length; ++i) {
        for (j = 0; j < *width; ++j) {
            image_array[i][j] = fgetc(fp);
        }
    }

    fclose(fp);

    return image_array;
}

int print_image_array(unsigned char **image_array, long length, long width) {
    int success = 0;
    int i;
    int j;

    for (i = 0; i < length; ++i) {
        for (j = 0; j < width; ++j) {
            printf("%d ", image_array[i][j]);
        }

        printf("\n");
    }

    return 1;
}
