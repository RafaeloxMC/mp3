#ifndef ST7735_H
#define ST7735_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void st7735_init(void);
void st7735_set_backlight(bool on);
void st7735_set_window(int x0, int y0, int x1, int y1);
void st7735_push_pixels(const uint16_t *pixels, size_t count);
void st7735_fill_rect(int x, int y, int w, int h, uint16_t color);
void st7735_draw_pixel(int x, int y, uint16_t color);
void st7735_draw_image_rgb565(int x, int y, int w, int h, const uint16_t *pixels);
void st7735_draw_char(int x, int y, char c, uint16_t fg, uint16_t bg, int scale);
void st7735_draw_text(int x, int y, const char *text, uint16_t fg, uint16_t bg, int scale);
int st7735_text_width(const char *text, int scale);

#define ST7735_BLACK 0x0000
#define ST7735_WHITE 0xFFFF
#define ST7735_RED 0xF800
#define ST7735_GREEN 0x07E0
#define ST7735_BLUE 0x001F
#define ST7735_YELLOW 0xFFE0
#define ST7735_CYAN 0x07FF
#define ST7735_GRAY 0x8410
#define ST7735_DARKGRAY 0x4208

#endif
