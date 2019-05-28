// See zak_oled_watch watch/draw.c
#include <mbed.h>

#include "draw.h"

Draw::Draw(Adafruit_SSD1306_Spi& display): _display(display), _textSize(1), _textColour(WHITE) {}

void Draw::drawString(char* string, bool invert, int16_t x, int16_t y) {
    _display.setTextSize(_textSize);
    _display.setTextColour(_textColour);
    _display.setCursor(x, y);
    _display.printf(string);
}
