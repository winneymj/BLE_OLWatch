
#include <mbed.h>
#include "notification.h"

Notification::Notification(NotificationBuffer& buf, Adafruit_SSD1306_Spi& display) : 
_notificationBuffer(buf), _display(display), _textSize(1), _textColour(WHITE) {}

void Notification::display() {
    _display.clearDisplay();
    // _display.display();

    // display.fillRect(0, 10, display.width(), display.height() - 10, BLACK); // Clear display
    _display.setTextSize(_textSize);            // Normal 1:1 pixel scale
    _display.setTextColour(_textColour);        // Draw white text
    _display.setCursor(0, 10);             // Start at top-left corner

    NotificationBuffer::iterator_t iterator;
    SharedPtr<MessageData> msgData;
    if (MBED_SUCCESS == _notificationBuffer.iterator_open(&iterator)) {

        while (_notificationBuffer.iterator_next(iterator, msgData)) {
            printf("msgData->subject%s\r\n", msgData->subject.get());
        }
    }

    // SharedPtr<MessageData> msgData;
    // // while (SharedPtr<MessageData> msgData = _notificationBuffer.next()) {
    //     _display.printf(msgData->subject.get());
    // // }
    _display.display();
}

void Notification::scrollUp() {
    printf("scrollUp: ENTER\r\n");

    _display.clearDisplay();
    _display.setTextSize(_textSize);            // Normal 1:1 pixel scale
    _display.setTextColour(_textColour);        // Draw white text
    _display.setCursor(0, 10);             // Start at top-left corner

    // See if any notifications and display "No Notifications" if none.
    if (_notificationBuffer.empty()) {
        _display.printf("No Notifications");
    }
}

void Notification::scrollDown() {

}

void Notification::setTextSize() {
    
}
