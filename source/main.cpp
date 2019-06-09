/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <platform/SharedPtr.h>
#include <events/mbed_events.h>
#include <mbed.h>
#include <PinNames.h>
#include <time.h>
#include "ble/BLE.h"
#include "SecurityManager.h"
#include "LEDService.h"
#include "SMDevice.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "dataFormat.h"
#include "typedefs.h"
#include "CircBuffer.h"
#include "notification.h"
#include "draw.h"
#include "normal.h"
#include "resources.h"

// #define PROGMEM
// #include "FreeMonoBold9pt7b.h"

/** This example demonstrates all the basic setup required
 *  for pairing and setting up link security both as a central and peripheral
 *
 *  The example is implemented as two classes, one for the peripheral and one
 *  for central inheriting from a common base. They are run in sequence and
 *  require a peer device to connect to.
 *
 *  During the test output is written on the serial connection to monitor its
 *  progress.
 */

extern time_t timeDate;

// define the Serial object
// Serial pc(USBTX, USBRX);
SPI mySPI(SPI_PSELMOSI0, NC, SPI_PSELSCK0, NC);
// Adafruit_SSD1306_Spi *display;
// Adafruit_SSD1306_Spi display(mySPI, P0_16, P0_17, P0_14 , 128, 64);
Draw myDisplay(mySPI, P0_16, P0_17, P0_14 , 128, 64);
events::EventQueue globalQueue;
// Circular buffer to hold history or messages
NotificationBuffer notificationBuffer;
Notification notificationDisplay(notificationBuffer, myDisplay);

// Normal watch face
Normal face(myDisplay);

// Button Interrupts
InterruptIn button1(P0_11);

int savedClearDown = -1;
int _savedFaceCall = -1;
int _periodCall = -1;

void testClearDownTimerCallback() {
    printf("testClearDownTimerCallback: ENTER\r\n");
    savedClearDown = -1;
    // Close the Notification display
    globalQueue.call(callback(&notificationDisplay, &Notification::close));
    printf("testClearDownTimerCallback: EXIT\r\n");
}

void messageCallback(SharedPtr<uint8_t> bufferPtr) {
    // printf("messageCallback: ENTER, bufferPtr=%s\r\n", bufferPtr.get());
    // Make sure UTF8 chars are convertes as best as possible to ASCII
    DataFormat::utf8ToAscii(bufferPtr);

    // Parse out the sender etc and return structure
    SharedPtr<MessageData> msgData = DataFormat::parseNotification(bufferPtr);

    // Only push data if we have a subject or body
    if (NULL != msgData->body || NULL != msgData->subject) {
        // Push to circular buffer
        notificationBuffer.push(msgData);

        // Now call the Notification display to display it.
        globalQueue.call(callback(&notificationDisplay, &Notification::displayCurrent));

        // Clear down after X seconds
        if (savedClearDown != -1) {
            globalQueue.cancel(savedClearDown);
        }
        savedClearDown = globalQueue.call_in(5000, callback(&testClearDownTimerCallback));
    }

    // display.fillRect(0, 10, display.width(), display.height() - 10, BLACK); // Clear display
    // display.setTextSize(1);             // Normal 1:1 pixel scale
    // display.setTextColour(WHITE);        // Draw white text
    // display.setCursor(0, 10);             // Start at top-left corner
    // display.printf(msgData->subject.get());
    // display.display();

    // printf("messageCallback.bufferPtr.get()=%ls\r\n", bufferPtr.get());
    printf("notificationBuffer.size=%ld\r\n", notificationBuffer.size());
    // Dump buffer
    // NotificationBuffer::iterator_t iterator;
    // SharedPtr<MessageData> msgData2;

    // if (MBED_SUCCESS == notificationBuffer.iterator_open(&iterator)) {
    //     while (MBED_SUCCESS == notificationBuffer.iterator_next(iterator, msgData2)) {
    //         printf("msgData->subject=%s\r\n", msgData2->subject.get());
    //     }
    // }
    // notificationBuffer.iterator_close(iterator);
}

void shutdownWatchFace() {
    // printf("timeDiff = %d\r\n", endTime - startTime);
    if (-1 != _savedFaceCall) {
        globalQueue.cancel(_savedFaceCall);
        _savedFaceCall = -1;
        myDisplay.clearDisplay();
        myDisplay.display();
    }
}

void handleButton1() {
    // Start displaying the watch face every X milliseconds
    _savedFaceCall = globalQueue.call_every(75, callback(&face, &Normal::displayWatchFace));

    // Start timer callback for 10 seconds to shutdown the watch face.
    _periodCall = globalQueue.call_in(10000, callback(&shutdownWatchFace));
}

void button1Trigger() {
    // If multiple clicks do not and the clear down
    // has not run then cancel the clear down.
    // if (savedClearDown != -1) {
    //     globalQueue.cancel(savedClearDown);
    // }
    // call the Notification scrollUp
    // globalQueue.call(callback(&notificationDisplay, &Notification::scrollUp));
    // savedClearDown = globalQueue.call_in(5000, callback(&testClearDownTimerCallback));

    // if (_savedFaceCallShutdown != -1) {
    //     globalQueue.cancel(_savedFaceCall);
    // }
    // _savedFaceCall = globalQueue.call_every(100, callback(&face, &Normal::displayWatchFace));
    // _savedFaceCall = globalQueue.call(callback(&face, &Normal::displayWatchFace));
    // _periodCall = globalQueue.call_in(10000, callback(&shutdownWatchFace));
    
    // Need to queue a call to handle the button as global.call_in() does 
    // not produce a reliable timed event.
    globalQueue.call(callback(handleButton1));
}

void setupButtonCallbacks() {
    button1.fall(callback(&button1Trigger));
    button1.mode(PullUp);
    button1.enable_irq();
}

int main() {
    printf("\r\n main: ENTER \r\n\r\n");

    mySPI.frequency(12000000);

    // Setup buttons
    setupButtonCallbacks();

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.

    // myDisplay.fillScreen(WHITE);
    // myDisplay.display();
    wait_ms(2000); // Pause for 2 seconds
    // Clear the buffer
    myDisplay.clearDisplay();

    printf("\r\n PERIPHERAL \r\n\r\n");

    // globalQueue.call_every(1000, callback(every1sec));

    // int8_t yPos = 0;
    // while (true) {
    //     uint8_t arraySize = (FONT_SMALL2_WIDTH * FONT_SMALL2_HEIGHT) / 8;
    //     const uint8_t* bitmap = small2Font[0 * arraySize];
    //     myDisplay.fastDrawBitmap(104, 28, bitmap, 
    //         FONT_SMALL2_WIDTH, FONT_SMALL2_HEIGHT, NOINVERT, yPos);
    //     myDisplay.display();
    //     yPos--;
    //     wait_ms(700);
    //     myDisplay.clearDisplay();
    // printf("ypos=%d\r\n", yPos);
    // }

    globalQueue.dispatch_forever();

    // BLE& ble = BLE::Instance();
    // SMDevicePeripheral peripheral(ble, globalQueue);
    // peripheral.run();

    printf("\r\n main: EXIT \r\n\r\n");
    return 0;
}