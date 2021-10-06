#ifndef _GFX_H
#define _GFX_H

///Modify to match used chip!
#include "stm32f1xx_hal.h"

uint16_t _width;
uint16_t _height;

int16_t cursor_x;     ///< x location to start print()ing text
int16_t cursor_y;     ///< y location to start print()ing text
uint16_t textcolor;   ///< 16-bit background color for print()
uint16_t textbgcolor; ///< 16-bit text color for print()
uint16_t textsize_x;
uint16_t textsize_y;

void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
		uint16_t bg, uint8_t size_x, uint8_t size_y);
void setCursor(int16_t x, int16_t y);
void setTextColor(uint16_t c, uint16_t bg);
void setTextSize(uint16_t s);

void writeChar(char c);
void printString(char s[]);

#endif /* _GFX_H */
