#ifndef __GRAPHICS_DRAW_H__
#define __GRAPHICS_DRAW_H__

#include <mbed.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

class Draw {
public:
    // ctor
    Draw(Adafruit_SSD1306_Spi& display);
    void drawString(char* string, bool invert, int16_t x, int16_t y);
    void clearDisplay() {_display.clearDisplay(); };
    void display() {_display.display(); };

private:
    Adafruit_SSD1306_Spi& _display;
    uint8_t _textSize;
    uint8_t _textColour;
};

#endif