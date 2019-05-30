#ifndef __GRAPHICS_DRAW_H__
#define __GRAPHICS_DRAW_H__

#include <mbed.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

class Draw: public Adafruit_SSD1306_Spi {
public:
    // ctor
    Draw(SPI &spi, PinName dc, PinName rst, PinName cs, uint8_t w, uint8_t h): Adafruit_SSD1306_Spi(spi, dc, rst, cs, w, h) {};
    void drawString(char* string, bool invert, int16_t x, int16_t y);
    void fastDrawBitmap(uint8_t x, uint8_t yy, const uint8_t* bitmap, uint8_t w, uint8_t h, bool invert, uint8_t offsetY);

private:
};

#endif