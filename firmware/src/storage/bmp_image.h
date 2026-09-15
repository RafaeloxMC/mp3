#ifndef BMP_IMAGE_H
#define BMP_IMAGE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int width;
    int height;
    uint16_t *pixels;
} bmp_image_t;

bool bmp_load(const char *path, bmp_image_t *out_image, int max_dimension);
void bmp_free(bmp_image_t *image);

#endif
