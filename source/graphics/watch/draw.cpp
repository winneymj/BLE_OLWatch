// See zak_oled_watch watch/draw.c
#include <mbed.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "draw.h"

extern uint8_t animation_offsetY();

void Draw::drawString(char* string, bool invert, int16_t x, int16_t y) {
    setCursor(x, y);
    printf(string);
}

// Ultra fast bitmap drawing
// Only downside is that height must be a multiple of 8, otherwise it gets rounded down to the nearest multiple of 8
// Drawing bitmaps that are completely on-screen and have a Y co-ordinate that is a multiple of 8 results in best performance
// PS - Sorry about the poorly named variables ;_;
void Draw::fastDrawBitmap(uint8_t x, uint8_t yy, const uint8_t* bitmap, uint8_t w, uint8_t h, bool invert, uint8_t offsetY) {
//Serial.println("-------fastDrawBitmap----------");
	uint8_t color = WHITE;

	// Apply animation offset
	yy += animation_offsetY();

// std::printf("fast, yy=%d\n\r", yy);
    // set up the pointer for fast movement through the buffer
    register uint8_t *pBuf = _frmbuf;

	// 
	int16_t y = yy - offsetY;

	// 
	uint8_t h2 = h / 8;
	
	// 
	uint8_t pixelOffset = (y % 8);

	uint8_t thing3 = (yy+h);
	
// std::printf("fast, h2=%d\n\r", h2);
	for (uint8_t hh = 0; hh < h2; hh++) {
// std::printf("fast, hh=%d\n\r", hh);
		// 
		uint8_t hhh = (hh * 8) + y; // Current Y pos (every 8 pixels)
		uint8_t hhhh = hhh + 8; // Y pos at end of pixel column (8 pixels)

		// 
		if(offsetY && (hhhh < yy || hhhh > HEIGHT || hhh > thing3))
			continue;

		// 
		uint8_t offsetMask = 0xFF;
		if(offsetY)
		{
			if(hhh < yy)
				offsetMask = (0xFF<<(yy-hhh));
			else if(hhhh > thing3)
				offsetMask = (0xFF>>(hhhh-thing3));
		}

		uint16_t aa = ((hhh / 8) * WIDTH);
		
		const uint8_t* b = bitmap + (hh * w);
		
		// If() outside of loop makes it faster (doesn't have to keep re-evaluating it)
		// Downside is code duplication
		if(!pixelOffset && hhh < HEIGHT)
		{
			// ww < w
            for (uint8_t ww = 0; ww < w; ww++ ) {
// std::printf("1 fast, ww=%d\n\r", ww);
			// LOOP(w, ww)
			// {
				// Workout X co-ordinate in frame buffer to place next 8 pixels
				uint8_t xx = ww + x;
			
				// Stop if X co-ordinate is outside the frame
				if(xx >= WIDTH)
					continue;

				// Read pixels
				// uint8_t pixels = readPixels(b + ww, invert) & offsetMask;
// std::printf("1 fast, b[%d]\n\r", ww);

                uint8_t pixels = (invert ? ~b[ww] : b[ww]) & offsetMask;

// std::printf("1 fast, pixels=0x%X\n\r", pixels);

				pBuf[xx + aa] |= pixels;
// display();
// wait_ms(2000);


				//setBuffByte(buff, xx, hhh, pixels, colour);
			}
		}
		else
		{
			uint16_t aaa = ((hhhh / 8) * WIDTH);
			
			// 
            for (uint8_t ww = 0; ww < w; ww++ ) {
// std::printf("2 fast, ww=%d\n\r", ww);
			// LOOP(w, ww)
			// {
				// Workout X co-ordinate in frame buffer to place next 8 pixels
				uint8_t xx = ww + x;
		
				// Stop if X co-ordinate is outside the frame
				if(xx >= WIDTH)
					continue;

				// Read pixels
                uint8_t pixels = (invert ? ~b[ww] : b[ww]) & offsetMask;
				// uint8_t pixels = readPixels(b + ww, invert) & offsetMask;

				// 
				if(hhh < HEIGHT)
					pBuf[xx + aa] |= pixels << pixelOffset;
					//setBuffByte(buff, xx, hhh, pixels << pixelOffset, colour);				

				// 
				if(hhhh < HEIGHT)
				{
					pBuf[xx + aaa] |= pixels >> (8 - pixelOffset);
					//setBuffByte(buff, xx, hhhh, pixels >> (8 - pixelOffset), colour);
// display();
// wait_ms(2000);
				}		
			}
		}
	}
}

