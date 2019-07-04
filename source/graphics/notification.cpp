
#include <mbed.h>
#define PROGMEM
#include "Adafruit_GFX\gfxfont.h"
//#include "Adafruit_GFX\fonts\FreeSans9pt7b.h"
#include "source\fonts\FreeSans6pt7b.h"
#include "notification.h"

Notification::Notification(NotificationBuffer& buf, Draw& display): 
_display(display), _notificationBuffer(buf), _textSize(1), _textColour(WHITE), _state(CLOSED), _scrollIterator(NULL) {}

void Notification::display(NotificationBuffer::iterator_t& iterator) {
    printf("display: ENTER\r\n");
    _display.clearDisplay();

    // display.fillRect(0, 10, display.width(), display.height() - 10, BLACK); // Clear display
    _display.setFont(&FreeSans6pt7b);
    // _display.setTextSize(_textSize);            // Normal 1:1 pixel scale
    _display.setTextColour(_textColour);        // Draw white text
    _display.setCursor(0, 10);             // Start at top-left corner

    // Iterator should be open before call to here
    SharedPtr<MessageData> msgData;
    // Get next item of buffer to display
    if (MBED_SUCCESS == _notificationBuffer.iterator_next(iterator, msgData)) {
        // _display.printf(msgData->subject.get());
        _display.drawString(msgData->subject.get(), false, 0, 10);
        printf("msgData->subject%s\r\n", msgData->subject.get());
    } else {
        // Nothing left in buffer
        // _display.printf("No more notifications");
        _display.drawString("No more notifications", false, 0, 10);
    }

    _display.display();

    // Set state to open as we are displaying
    _state = OPEN;

    printf("display: EXIT\r\n");
}

void Notification::scrollUp() {
    printf("scrollUp: ENTER\r\n");
    printf("scrollUp: _state=%s\r\n", (OPEN == _state) ? "OPEN" : "CLOSE");

    // See if any notifications and display "No Notifications" if none.
    if (_notificationBuffer.empty()) {
        _display.drawString("No Notifications", false, 0, 10);
        // _display.printf("No Notifications");
        _display.display();
    } else {
        // If state is closed then open and display first message.
        // If already open then go to next message
        if (CLOSED == _state) {
            _state = OPEN;
            // Open the buffer iterator
            SharedPtr<MessageData> msgData;
            if (MBED_SUCCESS == _notificationBuffer.iterator_open(&_scrollIterator)) {
                printf("scrollUp: iterator_open\r\n");
                display(_scrollIterator);
            } else {
                printf("scrollUp: iterator_open != MBED_SUCCESS\r\n");
            }
        } else {
            // Must be already displaying notification
            // so move to the next one and display
            display(_scrollIterator);
        }
    }
}

void Notification::scrollDown() {

}

void Notification::setTextSize() {
    
}

void Notification::displayCurrent() {
    printf("displayCurrent: ENTER\r\n");

    // Open the buffer iterator
    NotificationBuffer::iterator_t iterator;
    SharedPtr<MessageData> msgData;
    if (MBED_SUCCESS == _notificationBuffer.iterator_open(&iterator)) {
        display(iterator);
    } else {
        printf("displayCurrent(): iterator_open != MBED_SUCCESS\r\n");
    }
    _notificationBuffer.iterator_close(iterator);
    printf("displayCurrent: EXIT\r\n");
}

void Notification::close() {
    printf("close: ENTER\r\n");

    _display.clearDisplay();
    _display.display();
    _state = CLOSED;
    if (NULL != _scrollIterator) {
        _notificationBuffer.iterator_close(_scrollIterator);
        _scrollIterator = NULL;
    }
    printf("close: EXIT\r\n");
}
