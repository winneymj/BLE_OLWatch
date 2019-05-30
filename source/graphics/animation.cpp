
#include <mbed.h>

#define COMPILE_ANIMATIONS

#ifdef COMPILE_ANIMATIONS

#include "typedefs.h"

static anim_s animationStatus;

uint8_t animation_offsetY()
{
	return animationStatus.offsetY;
}

#endif