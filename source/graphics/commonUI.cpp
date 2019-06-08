
#include <mbed.h>
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

	_display.fastDrawBitmap(0, _display.height() - 8, battIcon, BATTERY_ICON_WIDTH, BATTERY_ICON_HEIGHT, NOINVERT, 0);
}
