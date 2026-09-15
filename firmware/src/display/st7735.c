#include "st7735.h"
#include "font5x7.h"
#include "config.h"

#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

#define CMD_SWRESET 0x01
#define CMD_SLPOUT 0x11
#define CMD_FRMCTR1 0xB1
#define CMD_FRMCTR2 0xB2
#define CMD_FRMCTR3 0xB3
#define CMD_INVCTR 0xB4
#define CMD_PWCTR1 0xC0
#define CMD_PWCTR2 0xC1
#define CMD_PWCTR3 0xC2
#define CMD_PWCTR4 0xC3
#define CMD_PWCTR5 0xC4
#define CMD_VMCTR1 0xC5
#define CMD_INVOFF 0x20
#define CMD_MADCTL 0x36
#define CMD_COLMOD 0x3A
#define CMD_CASET 0x2A
#define CMD_RASET 0x2B
#define CMD_RAMWR 0x2C
#define CMD_GMCTRP1 0xE0
#define CMD_GMCTRN1 0xE1
#define CMD_NORON 0x13
#define CMD_DISPON 0x29

static inline void cs_select(void) { gpio_put(PIN_SCREEN_CS, 0); }
static inline void cs_deselect(void) { gpio_put(PIN_SCREEN_CS, 1); }

static void write_command(uint8_t cmd) {
    gpio_put(PIN_SCREEN_DC, 0);
    cs_select();
    spi_write_blocking(SCREEN_SPI_PORT, &cmd, 1);
    cs_deselect();
}

static void write_data(const uint8_t *data, size_t len) {
    gpio_put(PIN_SCREEN_DC, 1);
    cs_select();
    spi_write_blocking(SCREEN_SPI_PORT, data, len);
    cs_deselect();
}

static void write_data_byte(uint8_t b) {
    write_data(&b, 1);
}

static void run_init_sequence(void) {
    write_command(CMD_SWRESET);
    sleep_ms(150);

    write_command(CMD_SLPOUT);
    sleep_ms(255);

    write_command(CMD_FRMCTR1);
    {
        uint8_t d[] = {0x01, 0x2C, 0x2D};
        write_data(d, sizeof(d));
    }

    write_command(CMD_FRMCTR2);
    {
        uint8_t d[] = {0x01, 0x2C, 0x2D};
        write_data(d, sizeof(d));
    }

    write_command(CMD_FRMCTR3);
    {
        uint8_t d[] = {0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D};
        write_data(d, sizeof(d));
    }

    write_command(CMD_INVCTR);
    write_data_byte(0x07);

    write_command(CMD_PWCTR1);
    {
        uint8_t d[] = {0xA2, 0x02, 0x84};
        write_data(d, sizeof(d));
    }

    write_command(CMD_PWCTR2);
    write_data_byte(0xC5);

    write_command(CMD_PWCTR3);
    {
        uint8_t d[] = {0x0A, 0x00};
        write_data(d, sizeof(d));
    }

    write_command(CMD_PWCTR4);
    {
        uint8_t d[] = {0x8A, 0x2A};
        write_data(d, sizeof(d));
    }

    write_command(CMD_PWCTR5);
    {
        uint8_t d[] = {0x8A, 0xEE};
        write_data(d, sizeof(d));
    }

    write_command(CMD_VMCTR1);
    write_data_byte(0x0E);

    write_command(CMD_INVOFF);

    write_command(CMD_MADCTL);
    write_data_byte(0xC8);

    write_command(CMD_COLMOD);
    write_data_byte(0x05);

    write_command(CMD_CASET);
    {
        uint8_t d[] = {0x00, 0x00, 0x00, 0x7F};
        write_data(d, sizeof(d));
    }

    write_command(CMD_RASET);
    {
        uint8_t d[] = {0x00, 0x00, 0x00, 0x9F};
        write_data(d, sizeof(d));
    }

    write_command(CMD_GMCTRP1);
    {
        uint8_t d[] = {0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d,
                       0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10};
        write_data(d, sizeof(d));
    }

    write_command(CMD_GMCTRN1);
    {
        uint8_t d[] = {0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
                       0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10};
        write_data(d, sizeof(d));
    }

    write_command(CMD_NORON);
    sleep_ms(10);

    write_command(CMD_DISPON);
    sleep_ms(100);
}

void st7735_init(void) {
    spi_init(SCREEN_SPI_PORT, SCREEN_SPI_BAUD_RATE_HZ);
    gpio_set_function(PIN_SCREEN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCREEN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_SCREEN_CS);
    gpio_set_dir(PIN_SCREEN_CS, GPIO_OUT);
    gpio_put(PIN_SCREEN_CS, 1);

    gpio_init(PIN_SCREEN_DC);
    gpio_set_dir(PIN_SCREEN_DC, GPIO_OUT);

    gpio_init(PIN_SCREEN_RESET);
    gpio_set_dir(PIN_SCREEN_RESET, GPIO_OUT);

    gpio_init(PIN_SCREEN_BACKLIGHT);
    gpio_set_dir(PIN_SCREEN_BACKLIGHT, GPIO_OUT);
    gpio_put(PIN_SCREEN_BACKLIGHT, 1);

    gpio_put(PIN_SCREEN_RESET, 1);
    sleep_ms(10);
    gpio_put(PIN_SCREEN_RESET, 0);
    sleep_ms(10);
    gpio_put(PIN_SCREEN_RESET, 1);
    sleep_ms(120);

    run_init_sequence();
}

void st7735_set_backlight(bool on) {
    gpio_put(PIN_SCREEN_BACKLIGHT, on ? 1 : 0);
}

void st7735_set_window(int x0, int y0, int x1, int y1) {
    write_command(CMD_CASET);
    {
        uint8_t d[] = {0x00, (uint8_t)x0, 0x00, (uint8_t)x1};
        write_data(d, sizeof(d));
    }
    write_command(CMD_RASET);
    {
        uint8_t d[] = {0x00, (uint8_t)y0, 0x00, (uint8_t)y1};
        write_data(d, sizeof(d));
    }
    write_command(CMD_RAMWR);
}

void st7735_push_pixels(const uint16_t *pixels, size_t count) {
    static uint8_t chunk[512];
    const size_t chunk_pixels = sizeof(chunk) / 2;

    gpio_put(PIN_SCREEN_DC, 1);
    cs_select();
    size_t offset = 0;
    while (offset < count) {
        size_t n = count - offset;
        if (n > chunk_pixels) n = chunk_pixels;
        for (size_t i = 0; i < n; i++) {
            uint16_t p = pixels[offset + i];
            chunk[i * 2] = (uint8_t)(p >> 8);
            chunk[i * 2 + 1] = (uint8_t)(p & 0xFF);
        }
        spi_write_blocking(SCREEN_SPI_PORT, chunk, n * 2);
        offset += n;
    }
    cs_deselect();
}

void st7735_fill_rect(int x, int y, int w, int h, uint16_t color) {
    if (w <= 0 || h <= 0) return;
    if (x < 0 || y < 0 || x + w > SCREEN_WIDTH || y + h > SCREEN_HEIGHT) return;

    st7735_set_window(x, y, x + w - 1, y + h - 1);

    uint8_t hi = (uint8_t)(color >> 8);
    uint8_t lo = (uint8_t)(color & 0xFF);
    uint8_t line[2 * 32];
    for (int i = 0; i < 32; i++) {
        line[i * 2] = hi;
        line[i * 2 + 1] = lo;
    }

    gpio_put(PIN_SCREEN_DC, 1);
    cs_select();
    int total = w * h;
    while (total > 0) {
        int chunk = total > 32 ? 32 : total;
        spi_write_blocking(SCREEN_SPI_PORT, line, chunk * 2);
        total -= chunk;
    }
    cs_deselect();
}

void st7735_draw_pixel(int x, int y, uint16_t color) {
    if (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return;
    st7735_set_window(x, y, x, y);
    st7735_push_pixels(&color, 1);
}

void st7735_draw_image_rgb565(int x, int y, int w, int h, const uint16_t *pixels) {
    if (x < 0 || y < 0 || x + w > SCREEN_WIDTH || y + h > SCREEN_HEIGHT) return;
    st7735_set_window(x, y, x + w - 1, y + h - 1);
    st7735_push_pixels(pixels, (size_t)(w * h));
}

#define GLYPH_CELL_W (FONT5X7_GLYPH_WIDTH + 1)
#define GLYPH_CELL_H 8
#define MAX_GLYPH_SCALE 4

void st7735_draw_char(int x, int y, char c, uint16_t fg, uint16_t bg, int scale) {
    if (scale < 1) scale = 1;
    if (scale > MAX_GLYPH_SCALE) scale = MAX_GLYPH_SCALE;
    unsigned char idx = (unsigned char)c;
    const unsigned char *glyph = &font5x7_table[idx * FONT5X7_GLYPH_WIDTH];

    static uint16_t cell[GLYPH_CELL_W * MAX_GLYPH_SCALE * GLYPH_CELL_H * MAX_GLYPH_SCALE];
    int cell_w = GLYPH_CELL_W * scale;
    int cell_h = GLYPH_CELL_H * scale;

    for (int row = 0; row < GLYPH_CELL_H; row++) {
        for (int col = 0; col < GLYPH_CELL_W; col++) {
            bool on = false;
            if (col < FONT5X7_GLYPH_WIDTH && row < 7) {
                on = (glyph[col] & (1 << row)) != 0;
            }
            uint16_t color = on ? fg : bg;
            for (int sy = 0; sy < scale; sy++) {
                for (int sx = 0; sx < scale; sx++) {
                    int px = col * scale + sx;
                    int py = row * scale + sy;
                    cell[py * cell_w + px] = color;
                }
            }
        }
    }

    if (x < 0 || y < 0 || x + cell_w > SCREEN_WIDTH || y + cell_h > SCREEN_HEIGHT) return;
    st7735_set_window(x, y, x + cell_w - 1, y + cell_h - 1);
    st7735_push_pixels(cell, (size_t)(cell_w * cell_h));
}

void st7735_draw_text(int x, int y, const char *text, uint16_t fg, uint16_t bg, int scale) {
    if (scale < 1) scale = 1;
    int cursor_x = x;
    int step = (FONT5X7_GLYPH_WIDTH + 1) * scale;
    while (*text) {
        st7735_draw_char(cursor_x, y, *text, fg, bg, scale);
        cursor_x += step;
        text++;
    }
}

int st7735_text_width(const char *text, int scale) {
    if (scale < 1) scale = 1;
    int len = 0;
    while (text[len]) len++;
    return len * (FONT5X7_GLYPH_WIDTH + 1) * scale;
}
