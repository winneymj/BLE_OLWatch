#ifndef __GRAPHICS_NORMAL_H__
#define __GRAPHICS_NORMAL_H__

#include <mbed.h>
#include "draw.h"
#include "typedefs.h"

typedef struct
{
	int8_t x;
	int8_t y;
	const uint8_t* bitmap;
	int8_t w;
	int8_t h;
	int8_t offsetY;
	int8_t val;
	int8_t maxVal;
	bool moving;
}tickerData_t;

class Normal {
public:
	Normal(Draw& display);
    display_t draw();
	void displayWatchFace();
	void shutdownDisplay();
	void halfSecond();

private:
	Draw& _display;
	bool _shutdown;
	bool _halfSecond;

    void drawDate();
    display_t ticker();
	void drawTickerNum(tickerData_t* data);
};

#endif