# SSD1306-Cube
*I2C SSD1306 Library for STM32Cube*\
Based on [Adafruit-SSD1306](https://github.com/adafruit/Adafruit_SSD1306) and [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
## Usage
This library can use DMA to write to the display. To enable it, configure DMA for the I2C peripheral associated to the display and uncomment line 7 in _ssd1306.h_

### Initializing the display
Before using the display, it must be initialized with _ssd1306Begin_.\
_ssd1306Begin_ takes four parameters: display mode, a pointer to the I2C Handler, width and height.\
_Example:_ `ssd1306Begin(SSD1306_SWITCHCAPVCC, &hi2c1, 128, 64);`

### Using the display

The functions used to draw to the display are similar to the ones used in [Adafruit-SSD1306](https://github.com/adafruit/Adafruit_SSD1306) and [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library) and are quite self-explanatory.

In _ssd1306.h_:\
`flushDisplay()` updates the display with framebuffer contents in memory.\
`clearDisplay()` clears the framebuffer without flushing it to the display.\
`void drawPixel(int16_t x, int16_t y, uint16_t color);`\
`void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);` \
`void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);`\
`void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);`\
In _gfx.h_:\
`void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);`\
`void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);`\
`void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);`\
`void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y);`\
`void setCursor(int16_t x, int16_t y);`\
`void setTextColor(uint16_t c, uint16_t bg);`\
`void setTextSize(uint16_t s);`\
`void writeChar(char c);` takes a single character and prints it at the current cursor position\
`void printString(char s[]);` takes a string and prints it at the current cursor position

If you need to redirect printf output to the display, add the following function to your code:
```c
int __io_putchar(int ch) {
	writeChar(ch);
    return ch;
}
```


