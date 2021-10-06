#include "ssd1306.h"
#include "gfx.h"

#define ssd1306_swap(a, b)                                                     \
(((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b))) ///< No-temp-var swap operation

uint8_t ssd1306Begin(uint8_t vccstate, I2C_HandleTypeDef *i2cdev) {
	_width = WIDTH;
	_height = HEIGHT;
	ssd1306Port = i2cdev;
	frameBuffer = (uint8_t*) malloc(WIDTH * ((HEIGHT + 7) / 8));
	clearDisplay();

	static const uint8_t init1[] = { SSD1306_DISPLAYOFF,         // 0xAE
			SSD1306_SETDISPLAYCLOCKDIV, // 0xD5
			0x80, // the suggested ratio 0x80
			SSD1306_SETMULTIPLEX }; // 0xA8
	ssd1306_commandList(init1, sizeof(init1));
	ssd1306_command1(HEIGHT - 1);

	static const uint8_t init2[] = { SSD1306_SETDISPLAYOFFSET, // 0xD3
			0x0,                      // no offset
			SSD1306_SETSTARTLINE | 0x0, // line #0
			SSD1306_CHARGEPUMP };        // 0x8D
	ssd1306_commandList(init2, sizeof(init2));

	ssd1306_command1((vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0x14);

	static const uint8_t init3[] = { SSD1306_MEMORYMODE, // 0x20
			0x00, // 0x0 act like ks0108
			SSD1306_SEGREMAP | 0x1,
			SSD1306_COMSCANDEC };
	ssd1306_commandList(init3, sizeof(init3));

	uint8_t comPins = 0x02;
	contrast = 0x8F;

	if ((WIDTH == 128) && (HEIGHT == 32)) {
		comPins = 0x02;
		contrast = 0x8F;
	} else if ((WIDTH == 128) && (HEIGHT == 64)) {
		comPins = 0x12;
		contrast = (vccstate == SSD1306_EXTERNALVCC) ? 0x9F : 0xCF;
	} else if ((WIDTH == 96) && (HEIGHT == 16)) {
		comPins = 0x2; // ada x12
		contrast = (vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0xAF;
	} else {
// Other screen varieties -- TBD
	}

	ssd1306_command1(SSD1306_SETCOMPINS);
	ssd1306_command1(comPins);
	ssd1306_command1(SSD1306_SETCONTRAST);
	ssd1306_command1(contrast);

	ssd1306_command1(SSD1306_SETPRECHARGE); // 0xd9
	ssd1306_command1((vccstate == SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);
	static const uint8_t init5[] = {
	SSD1306_SETVCOMDETECT, // 0xDB
			0x40,
			SSD1306_DISPLAYALLON_RESUME, // 0xA4
			SSD1306_NORMALDISPLAY,       // 0xA6
			SSD1306_DEACTIVATE_SCROLL,
			SSD1306_DISPLAYON }; // Main screen turn on
	ssd1306_commandList(init5, sizeof(init5));

}

void invertDisplay(uint8_t i) {
	ssd1306_command1(i ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
}

void clearDisplay(void) {
	memset(frameBuffer, 0, WIDTH * ((HEIGHT + 7) / 8));
}

uint8_t getRotation(void) {
	return rotation;
}

uint8_t width(void) {
	return WIDTH;
}

uint8_t height(void) {
	return HEIGHT;
}

void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w,
		int16_t h, uint16_t color) {
	int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
	uint8_t byte = 0;
	for (int16_t j = 0; j < h; j++, y++) {
		for (int16_t i = 0; i < w; i++) {
			if (i & 7)
				byte <<= 1;
			else
				byte = bitmap[j * byteWidth + i / 8];
			if (byte & 0x80)
				drawPixel(x + i, y, color);
		}
	}
}

void drawPixel(int16_t x, int16_t y, uint16_t color) {
	if ((x >= 0) && (x < width()) && (y >= 0) && (y < height())) {
// Pixel is in-bounds. Rotate coordinates if needed.
		switch (getRotation()) {
		case 1:
			ssd1306_swap(x, y);
			x = WIDTH - x - 1;
			break;
		case 2:
			x = WIDTH - x - 1;
			y = HEIGHT - y - 1;
			break;
		case 3:
			ssd1306_swap(x, y);
			y = HEIGHT - y - 1;
			break;
		}
		switch (color) {
		case SSD1306_WHITE:
			frameBuffer[x + (y / 8) * WIDTH] |= (1 << (y & 7));
			break;
		case SSD1306_BLACK:
			frameBuffer[x + (y / 8) * WIDTH] &= ~(1 << (y & 7));
			break;
		case SSD1306_INVERSE:
			frameBuffer[x + (y / 8) * WIDTH] ^= (1 << (y & 7));
			break;
		}
	}
}


void flushDisplay(void) {
	static const uint8_t dlist1[] = {
	SSD1306_PAGEADDR, 0,                      // Page start address
			0xFF,                   // Page end (not really, but works here)
			SSD1306_COLUMNADDR, 0 }; // Column start address
	ssd1306_commandList(dlist1, sizeof(dlist1));
	ssd1306_command1(WIDTH - 1); // Column end address

	uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
	uint8_t *ptr = frameBuffer;

#ifdef OLED_DMA
	HAL_I2C_Mem_Write_DMA(ssd1306Port, SSD1306_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT,
			ptr, count + 1);
	while (HAL_I2C_GetState(ssd1306Port) != HAL_I2C_STATE_READY);

#else
	HAL_I2C_Mem_Write(ssd1306Port, SSD1306_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT,
			ptr, count, HAL_MAX_DELAY);

#endif

}

void ssd1306_command1(uint8_t c) {
	HAL_I2C_Mem_Write(ssd1306Port, SSD1306_ADDR, 0x00, 1, &c, 1, HAL_MAX_DELAY);
}

void ssd1306_commandList(const uint8_t *c, uint8_t n) {
	HAL_I2C_Mem_Write(ssd1306Port, SSD1306_ADDR, 0x00, 1, c, n, HAL_MAX_DELAY);
}

void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
	uint8_t bSwap = 0;
	switch (rotation) {
	case 1:
// 90 degree rotation, swap x & y for rotation,
// then invert x and adjust x for h (now to become w)
		bSwap = 1;
		ssd1306_swap(x, y);
		x = WIDTH - x - 1;
		x -= (h - 1);
		break;
	case 2:
// 180 degree rotation, invert x and y, then shift y around for height.
		x = WIDTH - x - 1;
		y = HEIGHT - y - 1;
		y -= (h - 1);
		break;
	case 3:
// 270 degree rotation, swap x & y for rotation, then invert y
		bSwap = 1;
		ssd1306_swap(x, y);
		y = HEIGHT - y - 1;
		break;
	}

	if (bSwap)
		drawFastHLineInternal(x, y, h, color);
	else
		drawFastVLineInternal(x, y, h, color);
}

void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
	uint8_t bSwap = 0;
	switch (rotation) {
	case 1:
// 90 degree rotation, swap x & y for rotation, then invert x
		bSwap = 1;
		ssd1306_swap(x, y);
		x = WIDTH - x - 1;
		break;
	case 2:
// 180 degree rotation, invert x and y, then shift y around for height.
		x = WIDTH - x - 1;
		y = HEIGHT - y - 1;
		x -= (w - 1);
		break;
	case 3:
// 270 degree rotation, swap x & y for rotation,
// then invert y and adjust y for w (not to become h)
		bSwap = 1;
		ssd1306_swap(x, y);
		y = HEIGHT - y - 1;
		y -= (w - 1);
		break;
	}

	if (bSwap)
		drawFastVLineInternal(x, y, w, color);
	else
		drawFastHLineInternal(x, y, w, color);
}

void drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, uint16_t color) {

	if ((x >= 0) && (x < WIDTH)) { // X coord in bounds?
		if (__y < 0) {               // Clip top
			__h += __y;
			__y = 0;
		}
		if ((__y + __h) > HEIGHT) { // Clip bottom
			__h = (HEIGHT - __y);
		}
		if (__h > 0) { // Proceed only if height is now positive
// this display doesn't need ints for coordinates,
// use local byte registers for faster juggling
			uint8_t y = __y, h = __h;
			uint8_t *pBuf = &frameBuffer[(y / 8) * WIDTH + x];

// do the first partial byte, if necessary - this requires some masking
			uint8_t mod = (y & 7);
			if (mod) {
// mask off the high n bits we want to set
				mod = 8 - mod;
// note - lookup table results in a nearly 10% performance
// improvement in fill* functions
// uint8_t mask = ~(0xFF >> mod);
				static const uint8_t premask[8] = { 0x00, 0x80, 0xC0, 0xE0,
						0xF0, 0xF8, 0xFC, 0xFE };
				uint8_t mask = premask[mod];
// adjust the mask if we're not going to reach the end of this byte
				if (h < mod)
					mask &= (0XFF >> (mod - h));

				switch (color) {
				case SSD1306_WHITE:
					*pBuf |= mask;
					break;
				case SSD1306_BLACK:
					*pBuf &= ~mask;
					break;
				case SSD1306_INVERSE:
					*pBuf ^= mask;
					break;
				}
				pBuf += WIDTH;
			}

			if (h >= mod) { // More to go?
				h -= mod;
// Write solid bytes while we can - effectively 8 rows at a time
				if (h >= 8) {
					if (color == SSD1306_INVERSE) {
// separate copy of the code so we don't impact performance of
// black/white write version with an extra comparison per loop
						do {
							*pBuf ^= 0xFF; // Invert byte
							pBuf += WIDTH; // Advance pointer 8 rows
							h -= 8;        // Subtract 8 rows from height
						} while (h >= 8);
					} else {
// store a local value to work with
						uint8_t val = (color != SSD1306_BLACK) ? 255 : 0;
						do {
							*pBuf = val;   // Set byte
							pBuf += WIDTH; // Advance pointer 8 rows
							h -= 8;        // Subtract 8 rows from height
						} while (h >= 8);
					}
				}

				if (h) { // Do the final partial byte, if necessary
					mod = h & 7;
// this time we want to mask the low bits of the byte,
// vs the high bits we did above
// uint8_t mask = (1 << mod) - 1;
// note - lookup table results in a nearly 10% performance
// improvement in fill* functions
					static const uint8_t postmask[8] = { 0x00, 0x01, 0x03, 0x07,
							0x0F, 0x1F, 0x3F, 0x7F };
					uint8_t mask = postmask[mod];
					switch (color) {
					case SSD1306_WHITE:
						*pBuf |= mask;
						break;
					case SSD1306_BLACK:
						*pBuf &= ~mask;
						break;
					case SSD1306_INVERSE:
						*pBuf ^= mask;
						break;
					}
				}
			}
		} // endif positive height
	}   // endif x in bounds
}

void drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) {

	if ((y >= 0) && (y < HEIGHT)) { // Y coord in bounds?
		if (x < 0) {                  // Clip left
			w += x;
			x = 0;
		}
		if ((x + w) > WIDTH) { // Clip right
			w = (WIDTH - x);
		}
		if (w > 0) { // Proceed only if width is positive
			uint8_t *pBuf = &frameBuffer[(y / 8) * WIDTH + x], mask = 1
					<< (y & 7);
			switch (color) {
			case SSD1306_WHITE:
				while (w--) {
					*pBuf++ |= mask;
				}
				;
				break;
			case SSD1306_BLACK:
				mask = ~mask;
				while (w--) {
					*pBuf++ &= mask;
				}
				;
				break;
			case SSD1306_INVERSE:
				while (w--) {
					*pBuf++ ^= mask;
				}
				;
				break;
			}
		}
	}
}

