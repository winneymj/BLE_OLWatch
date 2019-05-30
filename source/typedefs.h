#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <mbed.h>
#include <platform/SharedPtr.h>
#include "CircBuffer.h"

typedef struct messageData {
    time_t time;
    SharedPtr<char> sender;
    SharedPtr<char> from;
    SharedPtr<char> subject;
    SharedPtr<char> body;
} MessageData;

#define BUF_SIZE 10
typedef CircBuffer<SharedPtr<MessageData>, BUF_SIZE> NotificationBuffer;

typedef struct{
	bool active;
	uint8_t offsetY;
	void (*animOnComplete)(void);
	bool goingOffScreen;
}anim_s;


#endif
