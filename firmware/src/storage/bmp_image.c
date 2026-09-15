#include "bmp_image.h"

#include <stdlib.h>
#include <string.h>

#include "ff.h"

static uint32_t read_u32le(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static int32_t read_i32le(const uint8_t *p) {
    return (int32_t)read_u32le(p);
}

static uint16_t read_u16le(const uint8_t *p) {
    return (uint16_t)(p[0] | (p[1] << 8));
}

bool bmp_load(const char *path, bmp_image_t *out_image, int max_dimension) {
    memset(out_image, 0, sizeof(*out_image));

    FIL file;
    if (f_open(&file, path, FA_READ) != FR_OK) {
        return false;
    }

    uint8_t header[54];
    UINT read_bytes = 0;
    if (f_read(&file, header, sizeof(header), &read_bytes) != FR_OK || read_bytes != sizeof(header)) {
        f_close(&file);
        return false;
    }

    if (header[0] != 'B' || header[1] != 'M') {
        f_close(&file);
        return false;
    }

    uint32_t data_offset = read_u32le(&header[10]);
    uint32_t dib_header_size = read_u32le(&header[14]);
    int32_t width = read_i32le(&header[18]);
    int32_t height_raw = read_i32le(&header[22]);
    uint16_t bpp = read_u16le(&header[28]);
    uint32_t compression = read_u32le(&header[30]);

    if (dib_header_size < 40 || compression != 0 || bpp != 24) {
        f_close(&file);
        return false;
    }

    bool top_down = height_raw < 0;
    int height = top_down ? -height_raw : height_raw;

    if (width <= 0 || height <= 0 || width > max_dimension || height > max_dimension) {
        f_close(&file);
        return false;
    }

    uint16_t *pixels = (uint16_t *)malloc((size_t)width * height * sizeof(uint16_t));
    if (!pixels) {
        f_close(&file);
        return false;
    }

    uint32_t row_bytes = (uint32_t)(((width * 3) + 3) / 4) * 4;
    uint8_t *row_buf = (uint8_t *)malloc(row_bytes);
    if (!row_buf) {
        free(pixels);
        f_close(&file);
        return false;
    }

    bool ok = true;
    for (int y = 0; y < height && ok; y++) {
        FSIZE_t offset = data_offset + (FSIZE_t)row_bytes * (FSIZE_t)y;
        if (f_lseek(&file, offset) != FR_OK) {
            ok = false;
            break;
        }
        if (f_read(&file, row_buf, row_bytes, &read_bytes) != FR_OK || read_bytes != row_bytes) {
            ok = false;
            break;
        }

        int dest_row = top_down ? y : (height - 1 - y);
        uint16_t *dest = &pixels[dest_row * width];
        for (int x = 0; x < width; x++) {
            uint8_t b = row_buf[x * 3 + 0];
            uint8_t g = row_buf[x * 3 + 1];
            uint8_t r = row_buf[x * 3 + 2];
            dest[x] = (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
        }
    }

    free(row_buf);
    f_close(&file);

    if (!ok) {
        free(pixels);
        return false;
    }

    out_image->width = width;
    out_image->height = height;
    out_image->pixels = pixels;
    return true;
}

void bmp_free(bmp_image_t *image) {
    if (image->pixels) {
        free(image->pixels);
        image->pixels = NULL;
    }
    image->width = 0;
    image->height = 0;
}
