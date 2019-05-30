// See zak_oled_watch watchfaces/normal.c
#include <mbed.h>

#include "resources.h"
#include "normal.h"

extern time_t timeDate;

Normal::Normal(Draw& display): _display(display) {};

void Normal::draw() {

    // Draw date
	drawDate();

	// Draw time animated
	// /*display_t busy = */ticker();

	// Draw battery icon
	// drawBattery();

    // @TODO USB and CHARGING

    // @DRAW NEXT ALARM 
}

void Normal::drawDate() {
	// // Get day string
	// char day[BUFFSIZE_STR_DAYS];
	// strcpy(day, days[timeDate.date.day]);

	// // Get month string
	// char month[BUFFSIZE_STR_MONTHS];
	// strcpy(month, months[timeDate.date.month]);

	// // Draw date
	// char buff[BUFFSIZE_DATE_FORMAT];
	// sprintf(buff, DATE_FORMAT, day, timeDate.date.date, month, timeDate.date.year);

	char buff[32] = {0};
    strftime(buff, 32, "%a, %b %e, %Y", localtime(&timeDate));	

    _display.setTextSize(1);
    _display.setTextColour(WHITE);
	_display.drawString(buff, false, 12, 0);
}

#define COMPILE_ANIMATIONS

// void/*display_t*/ Normal::ticker()
// {
// 	static int16_t yPos;
// 	static int16_t yPos_secs;
// 	static bool moving = false;
// 	static bool moving2[5];

// #if COMPILE_ANIMATIONS
// 	static byte hour2;
// 	static byte mins;
// 	static byte secs;

// 	if(appConfig.animations)
// 	{
// 		if(timeDate.time.secs != secs)
// 		{
// 			yPos = 0;
// 			yPos_secs = 0;
// 			moving = true;

// 			moving2[0] = div10(timeDate.time.hour) != div10(hour2);
// 			moving2[1] = mod10(timeDate.time.hour) != mod10(hour2);
// 			moving2[2] = div10(timeDate.time.mins) != div10(mins);
// 			moving2[3] = mod10(timeDate.time.mins) != mod10(mins);
// 			moving2[4] = div10(timeDate.time.secs) != div10(secs);
		
// 			//memcpy(&timeDateLast, &timeDate, sizeof(timeDate_s));
// 			hour2 = timeDate.time.hour;
// 			mins = timeDate.time.mins;
// 			secs = timeDate.time.secs;
// 		}

// 		if(moving)
// 		{
// 			if(yPos <= 3)
// 				yPos++;
// 			else if(yPos <= 6)
// 				yPos += 3;
// 			else if(yPos <= 16)
// 				yPos += 5;
// 			else if(yPos <= 22)
// 				yPos += 3;
// 			else if(yPos <= 24 + TICKER_GAP)
// 				yPos++;

// 			if(yPos >= MIDFONT_HEIGHT + TICKER_GAP)
// 				yPos = 255;

// 			if(yPos_secs <= 1)
// 				yPos_secs++;
// 			else if(yPos_secs <= 13)
// 				yPos_secs += 3;
// 			else if(yPos_secs <= 16 + TICKER_GAP)
// 				yPos_secs++;

// 			if(yPos_secs >= FONT_SMALL2_HEIGHT + TICKER_GAP)
// 				yPos_secs = 255;

// 			if(yPos_secs > FONT_SMALL2_HEIGHT + TICKER_GAP && yPos > MIDFONT_HEIGHT + TICKER_GAP)
// 			{
// 				yPos = 0;
// 				yPos_secs = 0;
// 				moving = false;
// 				memset(moving2, false, sizeof(moving2));
// 			}
// 		}
// 	}
// 	else
// #endif
// 	{
// 		yPos = 0;
// 		yPos_secs = 0;
// 		moving = false;
// 		memset(moving2, false, sizeof(moving2));
// 	}

// 	tickerData_t data;

// 	// Seconds
// 	data.x = 104;
// 	data.y = 28;
// 	data.bitmap = (const byte*)&small2Font;
// 	data.w = FONT_SMALL2_WIDTH;
// 	data.h = FONT_SMALL2_HEIGHT;
// 	data.offsetY = yPos_secs;
// 	data.val = div10(timeDate.time.secs);
// 	data.maxVal = 5;
// 	data.moving = moving2[4];
// 	drawTickerNum(&data);

// 	data.x = 116;
// 	data.val = mod10(timeDate.time.secs);
// 	data.maxVal = 9;
// 	data.moving = moving;
// 	drawTickerNum(&data);
	
// 	// Set new font data for hours and minutes
// 	data.y = TIME_POS_Y;
// 	data.w = MIDFONT_WIDTH;
// 	data.h = MIDFONT_HEIGHT;
// 	data.bitmap = (const byte*)&midFont;
// 	data.offsetY = yPos;

// 	// Minutes
// 	data.x = 60;
// 	data.val = div10(timeDate.time.mins);
// 	data.maxVal = 5;
// 	data.moving = moving2[2];
// 	drawTickerNum(&data);

// 	data.x = 83;
// 	data.val = mod10(timeDate.time.mins);
// 	data.maxVal = 9;
// 	data.moving = moving2[3];
// 	drawTickerNum(&data);

// 	// Hours
// 	data.x = 1;
// 	data.val = div10(timeDate.time.hour);
// 	data.maxVal = 5;
// 	data.moving = moving2[0];
// 	drawTickerNum(&data);

// 	data.x = 24;
// 	data.val = mod10(timeDate.time.hour);
// 	data.maxVal = 9;
// 	data.moving = moving2[1];
// 	drawTickerNum(&data);
	
// 	// Draw colon for half a second
// 	if(RTC_HALFSEC())
// 		draw_bitmap(TIME_POS_X + 46 + 2, TIME_POS_Y, colon, FONT_COLON_WIDTH, FONT_COLON_HEIGHT, NOINVERT, 0);
	
// 	// Draw AM/PM character
// 	char tmp[2];
// 	tmp[0] = timeDate.time.ampm;
// 	tmp[1] = 0x00;
// 	draw_string(tmp, false, 104, 20);

// //	char buff[12];
// //	sprintf_P(buff, PSTR("%lu"), time_getTimestamp());
// //	draw_string(buff, false, 30, 50);

// 	return (moving ? DISPLAY_BUSY : DISPLAY_DONE);
// }
