#ifndef __GRAPHICS_NOTIFICATION_H__
#define __GRAPHICS_NOTIFICATION_H__

#include <mbed.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "typedefs.h"

class Notification {
public:
    Notification(NotificationBuffer& buf, Adafruit_SSD1306_Spi& display);
    void display();
    void scrollUp();
    void scrollDown();
    void setTextSize();

private:
    NotificationBuffer& _notificationBuffer;
    Adafruit_SSD1306_Spi& _display;
    uint8_t _textSize;
    uint8_t _textColour;
};

#endif