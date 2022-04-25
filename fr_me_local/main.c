#include <stdio.h>
#include <string.h>

#include "extract.h"

int main(int argc, char *argv[]) {
    char name[80];
    long length;
    long width;

    unsigned char **image;

    if (argc < 2) {
        printf("Not enough parameters");
    }

    strcpy(name, argv[1]);

    image = read_grayscale_binary_file(name, &length, &width);

    // print_image_array(image, length, width);

    return 0;
}