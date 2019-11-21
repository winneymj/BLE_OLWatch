#ifndef _ADAFRUIT_ST7789H_
#define _ADAFRUIT_ST7789H_

#include "mbed.h"
#include "Adafruit_ST77xx.h"

// Subclass of ST77XX type display for ST7789 TFT Driver
class Adafruit_ST7789 : public Adafruit_ST77xx {
  public:
    Adafruit_ST7789(SPI &spiClass, PinName cs, PinName dc, PinName rst);
    void setRotation(uint8_t m);
    void init(uint16_t width, uint16_t height, uint8_t spiMode = SPI_MODE0);
};

#endif // _ADAFRUIT_ST7789H_
