#ifndef ST7789_H_
#define ST7789_H_

#include <stdint.h>


#define ST7789_WIDTH      240
#define ST7789_HEIGHT     240

#define ST7789_BLACK      0x0000
#define ST7789_WHITE      0xFFFF


void ST7789_Init(void);


void ST7789_FillScreen(
        uint16_t color);


void ST7789_FillRect(
        int16_t x,
        int16_t y,
        int16_t width,
        int16_t height,
        uint16_t color);


void ST7789_DrawString(
        int16_t x,
        int16_t y,
        const char *text,
        uint16_t color,
        uint8_t scale);


/*
 * Hàm mới:
 * vẽ chữ có cả màu nền
 */
void ST7789_DrawStringBG(
        int16_t x,
        int16_t y,
        const char *text,
        uint16_t color,
        uint16_t background,
        uint8_t scale);


#endif
