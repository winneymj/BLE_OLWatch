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

void messageCallback(SharedPtr<uint8_t> bufferPtr) {
    printf("messageCallback: ENTER\r\n");
const char* ptr = (char*)bufferPtr.get();
for (int x = 0; x < strlen(ptr); x++) {
    printf("0x%X,", ptr[x]);
}
    printf("\r\n");
    printf("messageCallback.bufferPtr.get()=%ls\r\n", bufferPtr.get());
}

int main() {
    printf("\r\n main: ENTER \r\n\r\n");
    mySPI.frequency(12000000);

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.display();
    wait_ms(2000); // Pause for 2 seconds
    // Clear the buffer
    display.clearDisplay();

    // mySPI.format(8, 1); // Mode 1.
    // mySPI.frequency(1000000);
    // display = new Adafruit_SSD1306_Spi(mySPI, P0_16, P0_17, P0_14 , 128, 64);

    printf("h1\r\n");
    BLE& ble = BLE::Instance();
    // events::EventQueue queue;

    // display.begin(SSD1306_SWITCHCAPVCC);
    printf("h2\r\n");
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