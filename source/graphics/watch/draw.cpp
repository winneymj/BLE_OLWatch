// See zak_oled_watch watch/draw.c
#include <mbed.h>
#include "Adafruit_GFX.h"
// #include "Adafruit_SSD1306.h"

#include "draw.h"

extern uint8_t animation_offsetY();

// void Draw::drawString(char* string, bool invert, int16_t x, int16_t y) {
// 	_rectX = x;
// 	_rectY = y;
// 	_rectW = _width;
// 	_rectH = _height;
//     setCursor(x, y);
//     printf(string);
// }

// void Draw::drawString(char* string, bool invert, int16_t x, int16_t y, int16_t w, int16_t h) {
// 	_rectX = x;
// 	_rectY = y;
// 	// Make sure _rectX + _rectW is not greater than _width and clamp 
// 	if ((_rectX + _rectW) > _width) {
// 		_rectW = _width - _rectX;
// 	} else {
// 		_rectW = w;
// 	}

// 	// Make sure _rectY + _rectW is not greater than _width and clamp 
// 	if ((_rectY + _rectH) > _height) {
// 		_rectY = _height - _rectY;
// 	} else {
// 		_rectH = h;
// 	}
//     setCursor(x, y);
//     printf(string);
// }

// // Ultra fast bitmap drawing
// // Only downside is that height must be a multiple of 8, otherwise it gets rounded down to the nearest multiple of 8
// // Drawing bitmaps that are completely on-screen and have a Y co-ordinate that is a multiple of 8 results in best performance
// // PS - Sorry about the poorly named variables ;_;
// void Draw::fastDrawBitmap(uint8_t x, uint8_t yy, const uint8_t* bitmap, uint8_t w, uint8_t h, bool invert, int8_t offsetY) {
// 	uint8_t color = WHITE;
// // std::printf("offsetY=%d\r\n", offsetY);
// 	// Apply animation offset
// 	yy += animation_offsetY();

//     // set up the pointer for fast movement through the buffer
//     register uint8_t *pBuf = _frmbuf;

// 	// 
// 	int16_t y = yy - offsetY;

// 	// 
// 	uint8_t h2 = h / 8;
	
// 	// 
// 	uint8_t pixelOffset = (y % 8);

// 	uint8_t thing3 = (yy+h);
	
// 	for (uint8_t hh = 0; hh < h2; hh++) {
// 		// 
// 		uint8_t hhh = (hh * 8) + y; // Current Y pos (every 8 pixels)
// 		uint8_t hhhh = hhh + 8; // Y pos at end of pixel column (8 pixels)

// 		// 
// 		if(offsetY && (hhhh < yy || hhhh > HEIGHT || hhh > thing3))
// 			continue;

// 		// 
// 		uint8_t offsetMask = 0xFF;
// 		if(offsetY)
// 		{
// 			if(hhh < yy)
// 				offsetMask = (0xFF<<(yy-hhh));
// 			else if(hhhh > thing3)
// 				offsetMask = (0xFF>>(hhhh-thing3));
// 		}

// 		uint16_t aa = ((hhh / 8) * WIDTH);
		
// 		const uint8_t* b = bitmap + (hh * w);
		
// 		// If() outside of loop makes it faster (doesn't have to keep re-evaluating it)
// 		// Downside is code duplication
// 		if(!pixelOffset && hhh < HEIGHT)
// 		{
// 			// ww < w
//             for (uint8_t ww = 0; ww < w; ww++ ) {
// 				// Workout X co-ordinate in frame buffer to place next 8 pixels
// 				uint8_t xx = ww + x;
			
// 				// Stop if X co-ordinate is outside the frame
// 				if(xx >= WIDTH)
// 					continue;

// 				// Read pixels
//                 uint8_t pixels = (invert ? ~b[ww] : b[ww]) & offsetMask;

// 				pBuf[xx + aa] |= pixels;
// 			}
// 		}
// 		else
// 		{
// 			uint16_t aaa = ((hhhh / 8) * WIDTH);
			
// 			// 
//             for (uint8_t ww = 0; ww < w; ww++ ) {
// 				// Workout X co-ordinate in frame buffer to place next 8 pixels
// 				uint8_t xx = ww + x;
		
// 				// Stop if X co-ordinate is outside the frame
// 				if(xx >= WIDTH)
// 					continue;

// 				// Read pixels
//                 uint8_t pixels = (invert ? ~b[ww] : b[ww]) & offsetMask;

// 				// 
// 				if(hhh < HEIGHT)
// 					pBuf[xx + aa] |= pixels << pixelOffset;

// 				// 
// 				if(hhhh < HEIGHT)
// 				{
// 					pBuf[xx + aaa] |= pixels >> (8 - pixelOffset);
// 				}		
// 			}
// 		}
// 	}
// }

// /**************************************************************************/
// /*!
//   @brief  Print one byte/character of data, used to support print()
//    This has been overriden from the Adafruit_GFX so that I can handle
//    Text areas using a rect(x, y, w, h) instead of setting the x to zero and
//    the width being the width of the display.  If I can be clever I will also
//    attempt to place a '...' at the end of the string if it is too long for
//    the text area.  Not sure if possible though right now.
//   @param  c       The 8-bit ascii character to write
// */
// /**************************************************************************/
// size_t Draw::writeChar(uint8_t c)
// {
//   if(!gfxFont)                      // 'Classic' built-in font
//   {
//     if(c == '\n')                   // Newline?
//     {
//       cursor_x = _rectX;            // Reset x to zero,
//       cursor_y += textsize * 8;     // advance y one line
//     }
//     else if(c != '\r')              // Ignore carriage returns
//     {
//       if(wrap && ((cursor_x + textsize * 6) > _rectX + _rectW))
//       {
//         cursor_x = _rectX;        // Reset x to zero,
//         cursor_y += textsize * 8;   // advance y one line
//       }
//       drawChar(cursor_x, cursor_y, c, textcolour, textbgcolour, textsize);
//       cursor_x += textsize * 6;     // Advance x one char
//     }
//   }
//   else                              // Custom font
//   {
//     if(c == '\n')
//     {
//       cursor_x = _rectX;
//       cursor_y += textsize * gfxFont->yAdvance;
//     }
//     else if(c != '\r')
//     {
//       uint8_t first = gfxFont->first;

//       if((c >= first) && (c <= gfxFont->last))
//       {
//         GFXglyph *glyph = (GFXglyph *) &gfxFont->glyph[c - first];
//         uint8_t w = glyph->width, h = glyph->height;
//         if((w > 0) && (h > 0))
//         { // Is there an associated bitmap?
//           int16_t xo = glyph->xOffset; // sic
//           if(wrap && ((cursor_x + textsize*(xo + w)) > _rectX + _rectW))
//           {
//             cursor_x = _rectX;
//             cursor_y += textsize * gfxFont->yAdvance;
//           }
// 		  // See if cursor y is outside the height
// 		  if (cursor_y <= (_rectY + _rectH)) {
// // std::printf("IN  cursor_y=%d\n\r", cursor_y);
// // std::printf("gfxFont->yAdvance=%d\n\r", gfxFont->yAdvance);
// 			// Guess move down as gfxFont->yAdvance / 2
// 			drawChar(cursor_x, cursor_y + (gfxFont->yAdvance / 2), c, textcolour, textbgcolour, textsize);
// 		  } else {
// // std::printf("OUT cursor_y=%d\n\r", cursor_y);
// 		  }
//         }
//         cursor_x += glyph->xAdvance * textsize;
//       }
//     }
//   }
//   return 1;
// }

