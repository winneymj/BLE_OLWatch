#ifndef __GRAPHICS_DRAW_H__
#define __GRAPHICS_DRAW_H__

#include <mbed.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ST7789.h"

class Draw: public Adafruit_ST7789 {
public:
    // ctor
    Draw(PinName mosi, PinName miso, PinName sck, PinName CS, PinName RS, PinName RST, uint8_t w, uint8_t h): Adafruit_ST7789(mosi, miso, sck, CS, RS, RST) {};
    // void drawString(char* string, bool invert, int16_t x, int16_t y);
    // void drawString(char* string, bool invert, int16_t x, int16_t y, int16_t w, int16_t h);
    // void fastDrawBitmap(uint8_t x, uint8_t yy, const uint8_t* bitmap, uint8_t w, uint8_t h, bool invert, int8_t offsetY);

    // virtual size_t writeChar(uint8_t);

private:
    // int16_t _rectX;
    // int16_t _rectY;
    // int16_t _rectW;
    // int16_t _rectH;
};

#endif