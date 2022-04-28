#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "imageio.h"

unsigned char **read_grayscale_binary_file(const char *filename, long *length, long *width) {
    FILE *fp;
    int i;
    int j;

    fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Output file could not be created or opened and rewritten.\n");
        return 0;
    }

    *length = 0;

    *length = fgetc(fp);
    (*length) <<= 8;
    *length ^= fgetc(fp);

    *width = 0;

    *width = fgetc(fp);
    (*width) <<= 8;
    *width ^= fgetc(fp);

    unsigned char **image_array = malloc(MAX_IMAGE_LENGTH * sizeof(unsigned char *));

    if (image_array == NULL) {
        printf("malloc of image_array failed\n");
        exit(1);
    }

    for (i = 0; i < *length; ++i) {
        image_array[i] = malloc(MAX_IMAGE_WIDTH * sizeof(unsigned char));

        if (image_array[i] == NULL) {
            printf("malloc of image_array[%d] failed\n", i);
            exit(1);
        }
    }

    for (i = 0; i < *length; ++i) {
        for (j = 0; j < *width; ++j) {
            int pixel_pos = (*width) * i + j;
            image_array[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH] = fgetc(fp);
        }
    }

    fclose(fp);

    return image_array;
}

int print_image_array(unsigned char **image_array, long length, long width, char output) {
    int success = 0;
    int i;
    int j;

    if (output == OUTPUT_TO_STDOUT) {
        for (i = 0; i < length; ++i) {
            for (j = 0; j < width; ++j) {
                int pixel_pos = width * i + j;
                printf("%d ", image_array[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH]);
            }

            printf("\n");
        }
    } else if (output == OUTPUT_TO_FILE) {
        FILE *fp;

        fp = fopen("binary_image_out.csv", "w+");

        if (fp == NULL) {
            printf("Output file could not be created or opened and rewritten.\n");
            return 0;
        }

        for (i = 0; i < length; ++i) {
            if (i) {
                fputc('\n', fp);
            }

            for (j = 0; j < width; ++j) {
                if (j) {
                    fputc(',', fp);
                }

                int pixel_pos = width * i + j;
                fprintf(fp, "%d", image_array[pixel_pos / MAX_IMAGE_WIDTH][pixel_pos % MAX_IMAGE_WIDTH]);
            }
        }

        fputc('\n', fp);

        fclose(fp);
    }

    success = 1;

    return success;
}
