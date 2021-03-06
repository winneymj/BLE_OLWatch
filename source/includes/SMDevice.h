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
#include "Adafruit_GFX.h"

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

// #define DEBUG_OUTPUT

static const uint8_t DEVICE_NAME[] = "SM_device";
static const uint16_t uuid16_list[] = {LEDService::LED_SERVICE_UUID};
static const uint8_t BT_TYPE_WRITE_SETUP = 0x00;
static const uint8_t BT_TYPE_WRITE_DIRECT = 0x04;
static const uint8_t DIRECT_WRITE_HEADER_SIZE = 0x03;

#define MAX_BUFFER_SIZE 136

LEDService *ledServicePtr;

extern void messageCallback(SharedPtr<uint8_t>);

/** Base class for both peripheral and central. The same class that provides
 *  the logic for the application also implements the SecurityManagerEventHandler
 *  which is the interface used by the Security Manager to communicate events
 *  back to the applications. You can provide overrides for a selection of events
 *  your application is interested in.
 */
class SMDevice : private mbed::NonCopyable<SMDevice>,
                 public SecurityManager::EventHandler
{

public:
    SMDevice(BLE &ble, events::EventQueue &event_queue) :
        _led1(LED1, 0),
        _ble(ble),
        _event_queue(event_queue),
        _handle(0),
        _is_connecting(false),
        _receiveTotalLength(0),
        _receiveBuffer(SharedPtr<uint8_t>(new uint8_t[MAX_BUFFER_SIZE + 1])),
        _msgReceiptTimer(-1) { };

    virtual ~SMDevice()
    {
        if (_ble.hasInitialized()) {
            _ble.shutdown();
        }
    };

    /** Start BLE interface initialisation */
    void run()
    {
#ifdef DEBUG_OUTPUT
        printf("SMDevice:run: ENTER\r\n");
#endif

        ble_error_t error;

        // /* to show we're running we'll blink every 500ms */
        // _event_queue.call_every(1000, this, &SMDevice::blink);

        if (_ble.hasInitialized()) {
            printf("Ble instance already initialised.\r\n");
            return;
        }

        /* this will inform us off all events so we can schedule their handling
         * using our event queue */
        _ble.onEventsToProcess(
            makeFunctionPointer(this, &SMDevice::schedule_ble_events)
        );

        error = _ble.init(this, &SMDevice::on_init_complete);

        if (error) {
            printf("Error returned by BLE::init.\r\n");
            return;
        }

        // /* this will not return until shutdown */
        // _event_queue.dispatch_forever();

#ifdef DEBUG_OUTPUT
        printf("SMDevice:run: EXIT\r\n");
#endif
    };

    /* event handler functions */

    /** Respond to a pairing request. This will be called by the stack
     * when a pairing request arrives and expects the application to
     * call acceptPairingRequest or cancelPairingRequest */
    virtual void pairingRequest(
        ble::connection_handle_t connectionHandle
    ) {
#ifdef DEBUG_OUTPUT
        printf("Pairing requested. Authorising.\r\n");
#endif
        _ble.securityManager().acceptPairingRequest(connectionHandle);
    }

    /** Inform the application of a successful pairing. Terminate the demonstration. */
    virtual void pairingResult(
        ble::connection_handle_t connectionHandle,
        SecurityManager::SecurityCompletionStatus_t result
    ) {
#ifdef DEBUG_OUTPUT
        printf("SMDevice:pairingResult: ENTER\r\n");
        if (result == SecurityManager::SEC_STATUS_SUCCESS) {
            printf("Pairing successful\r\n");
        } else {
            printf("Pairing failed\r\n");
        }
        // /* disconnect in 500 ms */
        // _event_queue.call_in(
        //     500, &_ble.gap(),
        //     &Gap::disconnect, _handle, Gap::REMOTE_USER_TERMINATED_CONNECTION
        // );
        printf("SMDevice:pairingResult: EXIT\r\n");
#endif
    }

    /** Inform the application of change in encryption status. This will be
     * communicated through the serial port */
    virtual void linkEncryptionResult(
        ble::connection_handle_t connectionHandle,
        ble::link_encryption_t result
    ) {
#ifdef DEBUG_OUTPUT
        printf("SMDevice:linkEncryptionResult: ENTER\r\n");
        if (result == ble::link_encryption_t::ENCRYPTED) {
            printf("Link ENCRYPTED\r\n");
        } else if (result == ble::link_encryption_t::ENCRYPTED_WITH_MITM) {
            printf("Link ENCRYPTED_WITH_MITM\r\n");
        } else if (result == ble::link_encryption_t::NOT_ENCRYPTED) {
            printf("Link NOT_ENCRYPTED\r\n");
        }
        printf("SMDevice:linkEncryptionResult: EXIT\r\n");
#endif
    }

private:
    /** Override to start chosen activity when initialisation completes */
    virtual void start() = 0;

    /**
     * This callback allows the LEDService to receive updates to the ledState Characteristic.
     *
     * @param[in] params
     *     Information about the characterisitc being updated.
     */
    void onDataWrittenCallback(const GattWriteCallbackParams *params) {
#ifdef DEBUG_OUTPUT
        printf("SMDevice:onDataWrittenCallback: ENTER\r\n");
        printf("SMDevice:onDataWrittenCallback: params->len=%d\r\n", params->len);
#endif
        if ((params->handle == ledServicePtr->getValueHandle())) {
            const uint8_t *dataPtr = params->data;

            // Get the message type
            const uint8_t messageType = (dataPtr[0] >> 4);

            if (BT_TYPE_WRITE_SETUP == messageType) {
                _receiveTotalLength = dataPtr[3];
                _receiveTotalLength = (_receiveTotalLength << 8) | dataPtr[2];
                _receiveTotalLength = (_receiveTotalLength << 8) | dataPtr[1];

                // fragments in block, number is send LSB
                _receiveTotalFragments = dataPtr[9];
                _receiveTotalFragments = (_receiveTotalFragments << 8) | dataPtr[8];
                _receiveTotalFragments = (_receiveTotalFragments << 8) | dataPtr[7];                

// printf("onDataWrittenCallback:before _receiveTotalLength=%d\r\n", _receiveTotalLength);
                // Limit total size to MAX_BUFFER_SIZE
                if (_receiveTotalLength > MAX_BUFFER_SIZE) {
                    _receiveTotalLength = MAX_BUFFER_SIZE;
                }
// printf("onDataWrittenCallback:after _receiveTotalLength=%d\r\n", _receiveTotalLength);

                // // Clear down buffer to null
                memset(_receiveBuffer.get(), 0, _receiveTotalLength + 1);

#ifdef DEBUG_OUTPUT
                printf("onDataWrittenCallback:BT_TYPE_WRITE_SETUP:_receiveTotalLength=%d,_receiveTotalFragments=%d\r\n", _receiveTotalLength, _receiveTotalFragments);
#endif
                // Setup a timer to wait for entire message.
                // If the timer expires before entire message then abandon
                // the buffer and reset for next.
                _msgReceiptTimer = _event_queue.call_in(5000, this, &SMDevice::msgTimeout);

            } else if (BT_TYPE_WRITE_DIRECT == messageType) {
                // payload length
                int16_t payloadLength = params->len - DIRECT_WRITE_HEADER_SIZE;

#ifdef DEBUG_OUTPUT
                printf("onDataWrittenCallback:BT_TYPE_WRITE_DIRECT:payloadLength=%d\r\n", payloadLength);
#endif
                // only continue if allocation was successful
                if (NULL != _receiveBuffer)
                {
                    // allocate reference counted dynamic memory buffer
                    // SharedPointer<BlockStatic> block(new BlockDynamic(buffer, payloadLength));

                    // set offset of the current block with regards to the overall characteristic
                    int32_t offset;
                    offset = dataPtr[2];
                    offset = (offset << 8) | dataPtr[1];

                    _receiveTotalFragments--;

                    // printf("onDataWrittenCallback:BT_TYPE_WRITE_DIRECT:memcpy, offset=%d,payloadLength=%d\r\n", (int)offset, (int)payloadLength);
                    // printf("onDataWrittenCallback:offset + payloadLength=%d\r\n", (int)offset + payloadLength);

                    // Make sure the payload fits in the buffer and thow away if not
                    if ((offset + payloadLength) <= MAX_BUFFER_SIZE) {
                        // copy payload
                        // printf("onDataWrittenCallback:COPY to [%d] len=%d\r\n", offset, payloadLength);
                        memcpy(&_receiveBuffer.get()[offset], (const void*)&dataPtr[3], payloadLength);
                    } else {
                        // printf("onDataWrittenCallback:THROW AWAY\r\n");
                    }

// for (int x = 0; x < _receiveTotalLength + 1; x++) {
//     printf("0x%X,", _receiveBuffer[x]);
// }
//     printf("\r\n");
                    // Count down fragments expected and when zero
                    // and buffer has some data then make callback.
                    // printf("onDataWrittenCallback:BT_TYPE_WRITE_DIRECT:_receiveTotalFragments=%d\r\n", _receiveTotalFragments);
                    if (_receiveTotalFragments == 0) {
                        // If total is now 0 then we got all of the message
                        // so cancel the timeout callback.
#ifdef DEBUG_OUTPUT
                        printf("onDataWrittenCallback:BT_TYPE_WRITE_DIRECT:_receiveBuffer=%s\r\n", _receiveBuffer);
#endif
                        _event_queue.cancel(_msgReceiptTimer);
                        _msgReceiptTimer = -1;

// printf("onDataWrittenCallback:_receiveTotalLength=%d\r\n", _receiveTotalLength);
// for (int x = 0; x < _receiveTotalLength + 1; x++) {
//     printf("0x%X,", _receiveBuffer[x]);
// }
// printf("\r\n");
                        // Add callback to event queue
                        // Create a char buffer to store and pass
                        SharedPtr<uint8_t> bufferPtr((uint8_t *)malloc(_receiveTotalLength + 1));
                        memcpy(bufferPtr.get(), _receiveBuffer.get(), _receiveTotalLength);
                        bufferPtr.get()[_receiveTotalLength] = 0; // Null last byte

                        Callback<void(SharedPtr<uint8_t>)> cb(messageCallback);
                        _event_queue.call(cb, bufferPtr);
                    }
                }                
            }
        }
#ifdef DEBUG_OUTPUT
        printf("SMDevice:onDataWrittenCallback: EXIT\r\n");
#endif
    }

    /** This is called when BLE interface is initialised and starts the demonstration */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *event)
    {
#ifdef DEBUG_OUTPUT
        printf("SMDevice:on_init_complete: ENTER\r\n");
#endif
        ble_error_t error;

        if (event->error) {
            printf("Error during the initialisation\r\n");
            return;
        }

        /* If the security manager is required this needs to be called before any
         * calls to the Security manager happen. */
        error = _ble.securityManager().init();

        if (error) {
            printf("Error during init %d\r\n", error);
            return;
        }

        /* Tell the security manager to use methods in this class to inform us
         * of any events. Class needs to implement SecurityManagerEventHandler. */
        _ble.securityManager().setSecurityManagerEventHandler(this);

        /* print device address */
        Gap::AddressType_t addr_type;
        Gap::Address_t addr;
        _ble.gap().getAddress(&addr_type, addr);
        printf("Device address: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
               addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);

        /* when scanning we want to connect to a peer device so we need to
         * attach callbacks that are used by Gap to notify us of events */
        _ble.gap().onConnection(this, &SMDevice::on_connect);
        _ble.gap().onDisconnection(this, &SMDevice::on_disconnect);

        /* handle timeouts, for example when connection attempts fail */
        _ble.gap().onTimeout().add(this, &SMDevice::on_timeout);

        _ble.gattServer().onDataWritten(this, &SMDevice::onDataWrittenCallback);
        
        // bool initialValueForLEDCharacteristic = false;
        // ledServicePtr = new LEDService(_ble, initialValueForLEDCharacteristic);
        ledServicePtr = new LEDService(_ble);

        /* start test in 500 ms */
        _event_queue.call_in(500, this, &SMDevice::start);

#ifdef DEBUG_OUTPUT
        printf("SMDevice:on_init_complete: EXIT\r\n");
#endif
    };

    /** This is called by Gap to notify the application we connected */
    virtual void on_connect(const Gap::ConnectionCallbackParams_t *connection_event) = 0;

    /** This is called by Gap to notify the application we disconnected,
     *  in our case it ends the demonstration. */
    void on_disconnect(const Gap::DisconnectionCallbackParams_t *event)
    {
#ifdef DEBUG_OUTPUT
        printf("SMDevice:on_disconnect: ENTER\r\n");
        printf("SMDevice:on_disconnect: Reason=0x%X\r\n", event->reason);
#endif
        // printf("Disconnected - demonstration ended \r\n");
        // _event_queue.break_dispatch();
        BLE::Instance().gap().startAdvertising();
#ifdef DEBUG_OUTPUT
        printf("SMDevice:on_disconnect: EXIT\r\n");
#endif
    };

    /** End demonstration unexpectedly. Called if timeout is reached during advertising,
     * scanning or connection initiation */
    void on_timeout(const Gap::TimeoutSource_t source)
    {
#ifdef DEBUG_OUTPUT
        printf("SMDevice:on_timeout: ENTER\r\n");
        printf("Unexpected timeout - aborting \r\n");
        // _event_queue.break_dispatch();
        printf("SMDevice:on_timeout: EXIT\r\n");
#endif
    };

    /** Schedule processing of events from the BLE in the event queue. */
    void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context)
    {
        // printf("SMDevice:schedule_ble_events: ENTER\r\n");
        _event_queue.call(mbed::callback(&context->ble, &BLE::processEvents));
        // printf("SMDevice:schedule_ble_events: EXIT\r\n");
    };

    /** 
     * Message has timed out, so we should start cleaning up and
     * resetting for next message.
     */
    void msgTimeout(void)
    {
#ifdef DEBUG_OUTPUT
        printf("SMDevice:msgTimeout: ENTER\r\n");
        printf("SMDevice:msgTimeout: EXIT\r\n");
#endif
    }

    /** Blink LED to show we're running */
//     void blink(void)
//     {
//         Gap::GapState_t gapState = _ble.gap().getState();
// #ifdef DEBUG_OUTPUT
//         printf("Gap State:Advertising=%s, connected=%s\r\n", gapState.advertising ? "On" : "Off", gapState.connected ? "Yes" : "No");
// #endif
//         // Solid led if advertising
//         if (gapState.advertising) {
//             display.fillCircle(8, 5, 5, WHITE);
//             _led1.write(0);
//         }
//         else if (gapState.connected) {
//             _led1 = !_led1;
//             display.fillCircle(8, 5, 5, _led1 ? BLACK : WHITE);
//         }

//         display.display(); // Update screen with each newly-drawn rectangle
// //----------------------------------------
//         // display.clearDisplay();

//         // for(int16_t i=0; i<display.height()/2; i+=2) {
//         //     display.drawRect(i, i, display.width()-2*i, display.height()-2*i, WHITE);
//         //     display.display(); // Update screen with each newly-drawn rectangle
//         //     wait_ms(1);
//         // }
// //----------------------------------------
//     };

private:
    DigitalOut _led1;
    uint16_t _receiveTotalLength; 
    uint16_t _receiveTotalFragments;
    // uint8_t *_receiveBuffer;
    SharedPtr<uint8_t> _receiveBuffer;
    int _msgReceiptTimer;

protected:
    BLE &_ble;
    events::EventQueue &_event_queue;
    ble::connection_handle_t _handle;
    bool _is_connecting;
};

/** A peripheral device will advertise, accept the connection and request
 * a change in link security. */
class SMDevicePeripheral : public SMDevice {
public:
    SMDevicePeripheral(BLE &ble, events::EventQueue &event_queue)
        : SMDevice(ble, event_queue) { }

    virtual void start()
    {
#ifdef DEBUG_OUTPUT
        printf("SMDevicePeripheral:start: ENTER\r\n");
#endif
        /* Set up and start advertising */

        ble_error_t error;
        GapAdvertisingData advertising_data;

        /* add advertising flags */
        advertising_data.addFlags(GapAdvertisingData::LE_GENERAL_DISCOVERABLE | GapAdvertisingData::BREDR_NOT_SUPPORTED);

        /* add device name */
        advertising_data.addData(GapAdvertisingData::COMPLETE_LOCAL_NAME, DEVICE_NAME, sizeof(DEVICE_NAME));

        advertising_data.addData(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));

        error = _ble.gap().setAdvertisingPayload(advertising_data);

        if (error) {
            printf("Error during Gap::setAdvertisingPayload\r\n");
            return;
        }

        /* advertise to everyone */
        _ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
        /* how many milliseconds between advertisements, lower interval
         * increases the chances of being seen at the cost of more power */
        _ble.gap().setAdvertisingInterval(5000);
        _ble.gap().setAdvertisingTimeout(0);

        error = _ble.gap().startAdvertising();

        if (error) {
            printf("Error during Gap::startAdvertising.\r\n");
            return;
        }

        /** This tells the stack to generate a pairingRequest event
         * which will require this application to respond before pairing
         * can proceed. Setting it to false will automatically accept
         * pairing. */
        _ble.securityManager().setPairingRequestAuthorisation(true);

#ifdef DEBUG_OUTPUT
        printf("SMDevicePeripheral:start: EXIT\r\n");
#endif
    };

    /** This is called by Gap to notify the application we connected,
     *  in our case it immediately requests a change in link security */
    virtual void on_connect(const Gap::ConnectionCallbackParams_t *connection_event)
    {
#ifdef DEBUG_OUTPUT
        printf("SMDevicePeripheral:on_connect: ENTER\r\n");
#endif
        ble_error_t error;

        /* store the handle for future Security Manager requests */
        _handle = connection_event->handle;

        /* Request a change in link security. This will be done
        * indirectly by asking the master of the connection to
        * change it. Depending on circumstances different actions
        * may be taken by the master which will trigger events
        * which the applications should deal with. */
        error = _ble.securityManager().setLinkSecurity(
            _handle,
            SecurityManager::SECURITY_MODE_ENCRYPTION_NO_MITM
        );

        if (error) {
            printf("Error during SM::setLinkSecurity %d\r\n", error);
            return;
        }
#ifdef DEBUG_OUTPUT
        printf("SMDevicePeripheral:on_connect: EXIT\r\n");
#endif
    };
};
