#ifndef __GRAPHICS_NOTIFICATION_H__
#define __GRAPHICS_NOTIFICATION_H__

#include <mbed.h>
#include "Adafruit_GFX.h"
// #include "Adafruit_SSD1306.h"
#include "typedefs.h"
#include "draw.h"

// class Notification {
// public:
//     enum eState {
//         OPEN,
//         CLOSED
//     };

//     Notification(NotificationBuffer& buf, Draw& display);
//     void display(NotificationBuffer::iterator_t& iterator);
//     void scrollUp();
//     void scrollDown();
//     void setTextSize();
//     void displayCurrent();
//     void close();

// private:
//     NotificationBuffer& _notificationBuffer;
//     Draw& _display;
//     uint8_t _textSize;
//     uint8_t _textColour;
//     eState _state;
//     NotificationBuffer::iterator_t _scrollIterator;
// };

#endif