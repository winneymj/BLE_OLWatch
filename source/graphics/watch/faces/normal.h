#ifndef __GRAPHICS_NORMAL_H__
#define __GRAPHICS_NORMAL_H__

#include <mbed.h>

typedef struct
{
	int8_t x;
	int8_t y;
	const int8_t* bitmap;
	int8_t w;
	int8_t h;
	int8_t offsetY;
	int8_t val;
	int8_t maxVal;
	bool moving;
}tickerData_t;

class Normal {
public:
    void draw();

private:    
    void drawDate();
    void/*display_t*/ ticker();
};

#endif