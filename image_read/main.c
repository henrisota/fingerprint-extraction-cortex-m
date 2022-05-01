#include "imageio.h"

int main(int argc, char *argv[]) {
    char name[80];
    long length;
    long width;

    short **image;
    struct bmpfileheader file_header;
    struct bitmapheader bm_header;

    if (argc < 2) {
        printf("Not enough parameters");
    }

    strcpy(name, argv[1]);

    get_image_size(name, &length, &width);

    image = allocate_image_array(length, width);

    read_image_array(name, image);

    // print_image_array(image, length, width);

    create_grayscale_binary_file(image, length, width);

    read_bmp_file_header(name, &file_header);

    // print_bmp_file_header(&file_header);

    read_bm_header(name, &bm_header);

    // print_bm_header(&bm_header);

    return 0;
}