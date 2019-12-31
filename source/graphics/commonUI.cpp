
#include <mbed.h>
#include "ble/BLE.h"

#include "typedefs.h"
#include "resources.h"
#include "draw.h"
#include "commonUI.h"

CommonUI::CommonUI(Draw& display): _display(display) {};

void CommonUI::drawBattery() {
	// uint battLevel = battery_voltage();
	uint16_t battLevel = 4001;
	const uint8_t* battIcon;
	if(battLevel < 3200)
		battIcon = battIconEmptyBitmaps;
	else if(battLevel < 3700)
		battIcon = battIconLowBitmaps;
	else if(battLevel < 4000)
		battIcon = battIconHighBitmaps;
	else
		battIcon = battIconFullBitmaps;

	// _display.fastDrawBitmap(0, _display.height() - 8, battIcon, BATTERY_ICON_WIDTH, BATTERY_ICON_HEIGHT, NOINVERT, 0);
}

// #define DEBUG_OUTPUT

void CommonUI::drawBLEState() {
	Gap::GapState_t gapState = BLE::Instance().gap().getState();
#ifdef DEBUG_OUTPUT
	printf("Gap State:Advertising=%s, connected=%s\r\n", gapState.advertising ? "On" : "Off", gapState.connected ? "Yes" : "No");
#endif
	// Solid led if advertising
	if (gapState.advertising) {
		// _display.fillCircle(3, 3, 3, WHITE);
		// _led1.write(0);
	}
	else if (gapState.connected) {
		// _led1 = !_led1;
		// _display.fillRect(1, 1, 5, 5, WHITE);
	}
}
