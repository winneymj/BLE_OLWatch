#ifndef __GRAPHICS_COMMONX_H__
#define __GRAPHICS_COMMONX_H__

#include <mbed.h>
#include "draw.h"

// extern Draw myDisplay;

class CommonUI {
public:
    CommonUI(Draw& display);
    void drawBattery();
private:
    Draw& _display;
};

#endif