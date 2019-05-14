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

// define the Serial object
// Serial pc(USBTX, USBRX);
SPI mySPI(SPI_PSELMOSI0, NC, SPI_PSELSCK0, NC);
// Adafruit_SSD1306_Spi *display;
Adafruit_SSD1306_Spi display(mySPI, P0_16, P0_17, P0_14 , 128, 64);
events::EventQueue globalQueue;
// Circular buffer to hold history or messages
NotificationBuffer notificationBuffer;
Notification notificationDisplay(notificationBuffer, display);

// Button Interrupts
InterruptIn button1(P0_11);

int savedClearDown = -1;

void messageCallback(SharedPtr<uint8_t> bufferPtr) {
    // printf("messageCallback: ENTER, bufferPtr=%s\r\n", bufferPtr.get());
    // Make sure UTF8 chars are convertes as best as possible to ASCII
    DataFormat::utf8ToAscii(bufferPtr);

    // Parse out the sender etc and return structure
    SharedPtr<MessageData> msgData = DataFormat::parseNotification(bufferPtr);

    // Push to circular buffer
    notificationBuffer.push(msgData);

    display.fillRect(0, 10, display.width(), display.height() - 10, BLACK); // Clear display
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColour(WHITE);        // Draw white text
    display.setCursor(0, 10);             // Start at top-left corner
    display.printf(msgData->subject.get());
    display.display();

    printf("messageCallback.bufferPtr.get()=%ls\r\n", bufferPtr.get());
    printf("notificationBuffer.size=%d\r\n", notificationBuffer.size());
    // Dump buffer
    NotificationBuffer::iterator_t iterator;
    SharedPtr<MessageData> msgData2;

    if (MBED_SUCCESS == notificationBuffer.iterator_open(&iterator)) {
        while (MBED_SUCCESS == notificationBuffer.iterator_next(iterator, msgData2)) {
            printf("msgData->subject=%s\r\n", msgData2->subject.get());
        }
    }
    notificationBuffer.iterator_close(iterator);
}

void testClearDownTimerCallback() {
    savedClearDown = -1;
    display.clearDisplay();
}

void button1Trigger() {
    // If multiple clicks do not and the clear down
    // has not run then cancel the clear down.
    if (savedClearDown != -1) {
        globalQueue.cancel(savedClearDown);
    }
    globalQueue.call(callback(&notificationDisplay, &Notification::scrollUp));
    savedClearDown = globalQueue.call_in(5000, callback(&testClearDownTimerCallback));
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
    display.display();
    wait_ms(2000); // Pause for 2 seconds
    // Clear the buffer
    display.clearDisplay();

    // mySPI.format(8, 1); // Mode 1.
    // mySPI.frequency(1000000);
    // display = new Adafruit_SSD1306_Spi(mySPI, P0_16, P0_17, P0_14 , 128, 64);

    BLE& ble = BLE::Instance();
    // events::EventQueue queue;

    // display.begin(SSD1306_SWITCHCAPVCC);
    // display.drawLine(1, 1, 127, 63, WHITE);
    // display.drawRect(1, 1, 127, 7, WHITE);
    // display.display();

    //  pc.printf("hahah");
    printf("\r\n PERIPHERAL \r\n\r\n");
    SMDevicePeripheral peripheral(ble, globalQueue);
    peripheral.run();

    printf("\r\n main: EXIT \r\n\r\n");
    return 0;
}