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
#include "LIS3DH.h"

#include "dataFormat.h"
#include "typedefs.h"
#include "CircBuffer.h"
#include "notification.h"
#include "draw.h"
#include "normal.h"
#include "resources.h"
#include "commonUI.h"

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

// I2C for accelerometer LIS3DH
I2C myI2C(P0_5, P0_4);

// Adafruit_SSD1306_Spi *display;
// Adafruit_SSD1306_Spi display(mySPI, P0_16, P0_17, P0_14 , 128, 64);
Draw myDisplay(mySPI, P0_16, P0_17, P0_14 , 128, 64);
events::EventQueue globalQueue;

// LIS3DH
#define LIS3DH_DEFAULT_ADDRESS (0x18)
LIS3DH myAccel (myI2C, LIS3DH_DEFAULT_ADDRESS);

// Circular buffer to hold history or messages
NotificationBuffer notificationBuffer;
Notification notificationDisplay(notificationBuffer, myDisplay);

// Normal watch face
Normal face(myDisplay);

CommonUI commonUI(myDisplay);

// BLE Handler
SMDevicePeripheral peripheral(BLE::Instance(), globalQueue);

// Button Interrupts
InterruptIn button1(P0_11);
// To Use P0_9 & P0_10 which are NFC allocated need to
// allow them to be used as GPIO by setting symbol
// CONFIG_NFCT_PINS_AS_GPIOS in the mbed_app.json
InterruptIn button2(P0_9);

// Vibration Motor P0_13
PwmOut vibMotor(P0_13);

int _savedClearDown = -1;
int _savedFaceCall = -1;
int _periodCall = -1;
int _savedColonCall = -1;

void testClearDownTimerCallback() {
    printf("testClearDownTimerCallback: ENTER\r\n");
    _savedClearDown = -1;
    // Close the Notification display
    globalQueue.call(callback(&notificationDisplay, &Notification::close));
    printf("testClearDownTimerCallback: EXIT\r\n");
}

void shutdownWatchFace() {
    // printf("timeDiff = %d\r\n", endTime - startTime);
    if (-1 != _savedFaceCall) {
        globalQueue.cancel(_savedFaceCall);
        _savedFaceCall = -1;
    }
    if (-1 != _periodCall) {
        globalQueue.cancel(_periodCall);
        _periodCall = -1;
    }
    if (-1 != _savedColonCall) {
        globalQueue.cancel(_savedColonCall);
        _savedColonCall = -1;
    }
    myDisplay.clearDisplay();
    myDisplay.display();
}

void startVibMotor() {
    vibMotor.write(0.3f);
}

void stopVibMotor() {
    vibMotor.write(1.0f);
}

#define PULSE_WIDTH_PERIOD_MS 400

void pulseVibrationMotor() {
    globalQueue.call(callback(startVibMotor));
    globalQueue.call_in(PULSE_WIDTH_PERIOD_MS, callback(stopVibMotor));
    // globalQueue.call_in(PULSE_WIDTH_PERIOD_MS * 2, callback(startVibMotor));
    // globalQueue.call_in(PULSE_WIDTH_PERIOD_MS * 3, callback(stopVibMotor));
}

void readAccellXYZ(/*float* x, float* y, float* z*/) {
    float data[3];
    myAccel.read_data(data);
    // *x = data[0];
    // *y = data[1];
    // *z = data[2];

    printf("readAccellXYZ, x=%f, y=%f, z=%f\r\n", data[0], data[1], data[2]);
}

void messageCallback(SharedPtr<uint8_t> bufferPtr) {
    printf("messageCallback: ENTER, bufferPtr=%s\r\n", bufferPtr.get());
    // Make sure UTF8 chars are convertes as best as possible to ASCII
    DataFormat::utf8ToAscii(bufferPtr);

    // Parse out the sender etc and return structure
    SharedPtr<MessageData> msgData = DataFormat::parseNotification(bufferPtr);

    // Only push data if we have a subject or body
    if (NULL != msgData->body || NULL != msgData->subject) {
        // Shutdown the watchface if it is displayed ready to display notification
        shutdownWatchFace();

        // Push to circular buffer
        notificationBuffer.push(msgData);

        // Now call the Notification display to display it.
        globalQueue.call(callback(&notificationDisplay, &Notification::displayCurrent));

        // Clear down after X seconds
        if (_savedClearDown != -1) {
            globalQueue.cancel(_savedClearDown);
        }
        _savedClearDown = globalQueue.call_in(5000, callback(&testClearDownTimerCallback));
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
    
    pulseVibrationMotor();
}

float dutyCycle = 0.0f;

void handleButton1() {
    shutdownWatchFace();

    // if (-1 != _savedFaceCall) {
    //     globalQueue.cancel(_savedFaceCall);
    // }
    // // Start displaying the watch face every X milliseconds
    _savedFaceCall = globalQueue.call_every(75, callback(&face, &Normal::displayWatchFace));

    // if (-1 != _savedColonCall) {
    //     globalQueue.cancel(_savedColonCall);
    // }
    // // Start displaying the time colon 500 milliseconds
    _savedColonCall = globalQueue.call_every(500, callback(&face, &Normal::halfSecond));

    // if (-1 != _periodCall) {
    //     globalQueue.cancel(_periodCall);
    // }
    // Start timer callback for 10 seconds to shutdown the watch face.
    _periodCall = globalQueue.call_in(10000, callback(&shutdownWatchFace));
}

void handleButton2() {
    printf("handleButton2\r\n");

    // Clear the watchface if displayed
    shutdownWatchFace();

    // Display the notifications if any.
    globalQueue.call(callback(&notificationDisplay, &Notification::displayCurrent));

    // Start timer callback for 10 seconds to shutdown the notification.
    _periodCall = globalQueue.call_in(10000, callback(&shutdownWatchFace));
}

void button1Trigger() {
    // If multiple clicks do not and the clear down
    // has not run then cancel the clear down.
    // if (_ savedClearDown != -1) {
    //     globalQueue.cancelsavedClearDown);
    // }
    // call the Notification scrollUp
    // globalQueue.call(callback(&notificationDisplay, &Notification::scrollUp));
    // _ savedClearDown = globalQueue.call_in(5000, callback(&testClearDownTimerCallback));

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

void button2Trigger() {
    // Need to queue a call to handle the button as global.call_in() does 
    // not produce a reliable timed event.
    globalQueue.call(callback(handleButton2));
}

void dummyMessages() {

    char *msgData = "com.android.vending||Checking for system and security updates|Checking for system and security updates";

printf("dummyMessages:strlen(msgData)=%d\r\n", strlen(msgData));

    SharedPtr<uint8_t> bufferPtr((uint8_t *)malloc(strlen(msgData) + 1));
    memcpy(bufferPtr.get(), msgData, strlen(msgData));
    bufferPtr.get()[strlen(msgData)] = 0; // Null last byte

    messageCallback(bufferPtr);
}

void setupButtonCallbacks() {
    button1.fall(callback(&button1Trigger));
    button1.mode(PullUp);
    button1.enable_irq();

    button2.fall(callback(&button2Trigger));
    button2.mode(PullUp);
    button2.enable_irq();
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

    // Place dummy notifications...remove once tested
    globalQueue.call_in(3000, callback(&dummyMessages));
    // float x, y, z;
    globalQueue.call_every(2000, callback(&readAccellXYZ));

    printf("\r\n PERIPHERAL \r\n\r\n");

    peripheral.run();

    globalQueue.dispatch_forever();

    printf("\r\n main: EXIT \r\n\r\n");
    return 0;
}