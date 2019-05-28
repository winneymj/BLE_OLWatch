#ifndef __GRAPHICS_NOTIFICATION_H__
#define __GRAPHICS_NOTIFICATION_H__

#include <mbed.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "typedefs.h"

class Notification {
public:
    enum eState {
        OPEN,
        CLOSED
    };

    Notification(NotificationBuffer& buf);
    void display(NotificationBuffer::iterator_t& iterator);
    void scrollUp();
    void scrollDown();
    void setTextSize();
    void displayCurrent();
    void close();

private:
    NotificationBuffer& _notificationBuffer;
    // Adafruit_SSD1306_Spi& _display;
    uint8_t _textSize;
    uint8_t _textColour;
    eState _state;
    NotificationBuffer::iterator_t _scrollIterator;
};

#endif