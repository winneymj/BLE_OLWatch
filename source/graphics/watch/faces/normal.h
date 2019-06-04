#ifndef __GRAPHICS_NORMAL_H__
#define __GRAPHICS_NORMAL_H__

#include <mbed.h>
#include "draw.h"

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
    void draw();

private:
	Draw& _display;
    void drawDate();
    void/*display_t*/ ticker();
	void drawTickerNum(tickerData_t* data);
};

#endif