#ifndef _SSD1306_H
#define _SSD1306_H

///Modify to match used chip!
#include "stm32f1xx_hal.h"

//Uncomment this to enable DMA
//#define OLED_DMA


//Commands
#define BLACK SSD1306_BLACK     ///< Draw 'off' pixels
#define WHITE SSD1306_WHITE     ///< Draw 'on' pixels
#define INVERSE SSD1306_INVERSE ///< Invert pixels

/// fit into the SSD1306_ naming scheme
#define SSD1306_BLACK 0   ///< Draw 'off' pixels
#define SSD1306_WHITE 1   ///< Draw 'on' pixels
#define SSD1306_INVERSE 2 ///< Invert pixels

#define SSD1306_MEMORYMODE 0x20          ///< See datasheet
#define SSD1306_COLUMNADDR 0x21          ///< See datasheet
#define SSD1306_PAGEADDR 0x22            ///< See datasheet
#define SSD1306_SETCONTRAST 0x81         ///< See datasheet
#define SSD1306_CHARGEPUMP 0x8D          ///< See datasheet
#define SSD1306_SEGREMAP 0xA0            ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SSD1306_DISPLAYALLON 0xA5        ///< Not currently used
#define SSD1306_NORMALDISPLAY 0xA6       ///< See datasheet
#define SSD1306_INVERTDISPLAY 0xA7       ///< See datasheet
#define SSD1306_SETMULTIPLEX 0xA8        ///< See datasheet
#define SSD1306_DISPLAYOFF 0xAE          ///< See datasheet
#define SSD1306_DISPLAYON 0xAF           ///< See datasheet
#define SSD1306_COMSCANINC 0xC0          ///< Not currently used
#define SSD1306_COMSCANDEC 0xC8          ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET 0xD3    ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5  ///< See datasheet
#define SSD1306_SETPRECHARGE 0xD9        ///< See datasheet
#define SSD1306_SETCOMPINS 0xDA          ///< See datasheet
#define SSD1306_SETVCOMDETECT 0xDB       ///< See datasheet

#define SSD1306_SETLOWCOLUMN 0x00  ///< Not currently used
#define SSD1306_SETHIGHCOLUMN 0x10 ///< Not currently used
#define SSD1306_SETSTARTLINE 0x40  ///< See datasheet

#define SSD1306_EXTERNALVCC 0x01  ///< External display voltage source
#define SSD1306_SWITCHCAPVCC 0x02 ///< Gen. display voltage from 3.3V

#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26              ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27               ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A  ///< Init diag scroll
#define SSD1306_DEACTIVATE_SCROLL 0x2E                    ///< Stop scroll
#define SSD1306_ACTIVATE_SCROLL 0x2F                      ///< Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3             ///< Set scroll range

#define WIDTH 128 
#define HEIGHT 64 

//I2C address and port
static const uint8_t SSD1306_ADDR = 0x3C << 1; // Use 8-bit address
I2C_HandleTypeDef *ssd1306Port;

//Framebuffer
uint8_t *frameBuffer;
uint8_t rotation;

//Init function
uint8_t ssd1306Begin(uint8_t vccstate, I2C_HandleTypeDef *i2cdev);
uint8_t contrast;

//Command functions
void ssd1306_command1(uint8_t c);
void ssd1306_commandList(const uint8_t *c, uint8_t n);
void invertDisplay(uint8_t i);

//Graphics Functions
void flushDisplay(void);
void clearDisplay(void);

void drawPixel(int16_t x, int16_t y, uint16_t color);
void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w,
		int16_t h, uint16_t color);
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, uint16_t color);
void drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color);

uint8_t width(void);
uint8_t height(void);
uint8_t getRotation(void);

#endif /* _SSD1306_H */
