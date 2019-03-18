#include "mbed.h"

#include "Adafruit_GFX.h"
#include "font5x7.h"

/**************************************************************************/
/*!
  @brief  Instantiate a GFX context for graphics! Can only be done by a superclass
  @param  w       Display width, in pixels
  @param  h       Display height, in pixels
*/
/**************************************************************************/
Adafruit_GFX::Adafruit_GFX(int16_t w, int16_t h) : WIDTH(w), HEIGHT(h)
{
  _width = WIDTH;
  _height = HEIGHT;
  rotation = 0;
  cursor_x = 0;
  cursor_y = 0;
  textsize = 1;
  textbgcolour = 0xFFFF;
  textcolour = 0xFFFF;
  wrap = true;
  _cp437 = false;
  gfxFont = NULL;
}

int Adafruit_GFX::_putc(int v)
{
  return writeChar(v);
};

int Adafruit_GFX::_getc(void)
{
  return -1;
};

/**************************************************************************/
/*!
  @brief  Write a line.  Bresenham's algorithm - thx wikpedia
  @param  x0      Start point x coordinate
  @param  y0      Start point y coordinate
  @param  x1      End point x coordinate
  @param  y1      End point y coordinate
  @param  colour  16-bit 5-6-5 colour to draw with
*/
/**************************************************************************/
void Adafruit_GFX::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour)
{
  int16_t steep = abs(y1-y0) > abs(x1-x0);

  if(steep)
  {
    swap(x0, y0);
    swap(x1, y1);
  }

  if(x0 > x1)
  {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx = x1-x0;
  int16_t dy = abs(y1-y0);
  int16_t err = dx/2;
  int16_t ystep;

  if(y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for(; x0 <= x1; x0++)
  {
    if(steep)
    {
      writePixel(y0, x0, colour);
    }
    else
    {
      writePixel(x0, y0, colour);
    }
    err -= dy;
    if(err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

/**************************************************************************/
/*!
  @brief  Start a display-writing routine, overwrite in subclasses.
*/
/**************************************************************************/
void Adafruit_GFX::startWrite(void)
{
}

/**************************************************************************/
/*!
  @brief  Write a pixel, overwrite in subclasses if startWrite is defined!
  @param  x       x-coordinate
  @param  y       y-coordinate
  @param  colour  16-bit 5-6-5 colour to fill with
*/
/**************************************************************************/
void Adafruit_GFX::writePixel(int16_t x, int16_t y, uint16_t colour)
{
  drawPixel(x, y, colour);
}

/**************************************************************************/
/*!
  @brief  Write a perfectly vertical line, overwrite in subclasses if startWrite is defined!
  @param  x       Top-most x-coordinate
  @param  y       Top-most y-coordinate
  @param  h       Height in pixels
  @param  colour  16-bit 5-6-5 colour to fill with
*/
/**************************************************************************/
void Adafruit_GFX::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t colour)
{
  // Overwrite in subclasses if startWrite is defined!
  // Can be just writeLine(x, y, x, y+h-1, colour);
  // or writeFillRect(x, y, 1, h, colour);
  drawFastVLine(x, y, h, colour);
}

/**************************************************************************/
/*!
  @brief  Write a perfectly horizontal line, overwrite in subclasses if startWrite is defined!
  @param  x       Left-most x coordinate
  @param  y       Left-most y coordinate
  @param  w       Width in pixels
  @param  colour  16-bit 5-6-5 colour to fill with
*/
/**************************************************************************/
void Adafruit_GFX::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t colour)
{
  // Overwrite in subclasses if startWrite is defined!
  // Example: writeLine(x, y, x+w-1, y, colour);
  // or writeFillRect(x, y, w, 1, colour);
  drawFastHLine(x, y, w, colour);
}

/**************************************************************************/
/*!
  @brief  Write a rectangle completely with one colour, overwrite in subclasses if startWrite is defined!
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  w       Width in pixels
  @param  h       Height in pixels
  @param  colour  16-bit 5-6-5 colour to fill with
*/
/**************************************************************************/
void Adafruit_GFX::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colour)
{
  // Overwrite in subclasses if desired!
  fillRect(x, y, w, h, colour);
}

/**************************************************************************/
/*!
  @brief  End a display-writing routine, overwrite in subclasses if startWrite is defined!
*/
/**************************************************************************/
void Adafruit_GFX::endWrite(void)
{
}

/**************************************************************************/
/*!
  @brief  Draw a perfectly vertical line (this is often optimized in a subclass!)
  @param  x       Top-most x-coordinate
  @param  y       Top-most y-coordinate
  @param  h       Height in pixels
  @param  colour  16-bit 5-6-5 colour to fill with
*/
/**************************************************************************/
void Adafruit_GFX::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t colour)
{
  startWrite();
  writeLine(x, y, x, y+h-1, colour);
  endWrite();
}

/**************************************************************************/
/*!
  @brief  Draw a perfectly horizontal line (this is often optimized in a subclass!)
  @param  x       Left-most x-coordinate
  @param  y       Left-most y-coordinate
  @param  w       Width in pixels
  @param  colour  16-bit 5-6-5 colour to fill with
*/
/**************************************************************************/
void Adafruit_GFX::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t colour)
{
  startWrite();
  writeLine(x, y, x+w-1, y, colour);
  endWrite();
}

/**************************************************************************/
/*!
  @brief  Fill a rectangle completely with one colour. Update in subclasses if desired!
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  w       Width in pixels
  @param  h       Height in pixels
  @param  colour  16-bit 5-6-5 colour to fill with
*/
/**************************************************************************/
void Adafruit_GFX::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colour)
{
  startWrite();
  for(int16_t i = x; i < x+w; i++)
  {
    writeFastVLine(i, y, h, colour);
  }
  endWrite();
}

/**************************************************************************/
/*!
   @brief    Fill the screen completely with one colour. Update in subclasses if desired!
    @param    colour 16-bit 5-6-5 colour to fill with
*/
/**************************************************************************/
void Adafruit_GFX::fillScreen(uint16_t colour)
{
  fillRect(0, 0, _width, _height, colour);
}

/**************************************************************************/
/*!
   @brief    Draw a line
    @param    x0  Start point x coordinate
    @param    y0  Start point y coordinate
    @param    x1  End point x coordinate
    @param    y1  End point y coordinate
    @param    colour 16-bit 5-6-5 colour to draw with
*/
/**************************************************************************/
void Adafruit_GFX::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour)
{
  // Update in subclasses if desired!
  if(x0 == x1)
  {
    if(y0 > y1)
    {
      swap(y0, y1);
    }
    drawFastVLine(x0, y0, y1-y0+1, colour);
  }
  else if(y0 == y1)
  {
    if(x0 > x1)
    {
      swap(x0, x1);
    }
    drawFastHLine(x0, y0, x1-x0+1, colour);
  }
  else
  {
    startWrite();
    writeLine(x0, y0, x1, y1, colour);
    endWrite();
  }
}

/**************************************************************************/
/*!
   @brief    Draw a circle outline
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    colour 16-bit 5-6-5 colour to draw with
*/
/**************************************************************************/
void Adafruit_GFX::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t colour)
{
  int16_t f = 1-r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2*r;
  int16_t x = 0;
  int16_t y = r;

  startWrite();
  writePixel(x0, y0+r, colour);
  writePixel(x0, y0-r, colour);
  writePixel(x0+r, y0, colour);
  writePixel(x0-r, y0, colour);

  while(x < y)
  {
    if(f >= 0)
    {
        y--;
        ddF_y += 2;
        f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    writePixel(x0+x, y0+y, colour);
    writePixel(x0-x, y0+y, colour);
    writePixel(x0+x, y0-y, colour);
    writePixel(x0-x, y0-y, colour);
    writePixel(x0+y, y0+x, colour);
    writePixel(x0-y, y0+x, colour);
    writePixel(x0+y, y0-x, colour);
    writePixel(x0-y, y0-x, colour);
  }
  endWrite();
}

/**************************************************************************/
/*!
  @brief  Quarter-circle drawer, used to do circles and roundrects
  @param  x0      Center-point x coordinate
  @param  y0      Center-point y coordinate
  @param  r       Radius of circle
  @param  quad    Mask bit #1 or bit #2 to indicate which quadrant of the circle we're doing
  @param  colour  16-bit 5-6-5 colour to draw with
*/
/**************************************************************************/
void Adafruit_GFX::drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t quad, uint16_t colour)
{
  int16_t f = 1-r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2*r;
  int16_t x = 0;
  int16_t y = r;

  while(x < y)
  {
    if(f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if(quad&0x1)
    {
      writePixel(x0-y, y0-x, colour);
      writePixel(x0-x, y0-y, colour);
    }
    if(quad&0x2)
    {
      writePixel(x0+x, y0-y, colour);
      writePixel(x0+y, y0-x, colour);
    }
    if(quad&0x4)
    {
      writePixel(x0+x, y0+y, colour);
      writePixel(x0+y, y0+x, colour);
    }
    if(quad&0x8)
    {
      writePixel(x0-y, y0+x, colour);
      writePixel(x0-x, y0+y, colour);
    }
  }
}

/**************************************************************************/
/*!
    @brief  Draw a circle with filled colour
    @param  x0      Center-point x coordinate
    @param  y0      Center-point y coordinate
    @param  r       Radius of circle
    @param  colour  16-bit 5-6-5 colour to fill with
*/
/**************************************************************************/
void Adafruit_GFX::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t colour)
{
  startWrite();
  writeFastVLine(x0, y0-r, 2*r+1, colour);
  fillCircleHelper(x0, y0, r, 0x3, 0, colour);
  endWrite();
}

/**************************************************************************/
/*!
    @brief  Quarter-circle drawer with fill, used for circles and roundrects
    @param  x0      Center-point x coordinate
    @param  y0      Center-point y coordinate
    @param  r       Radius of circle
    @param  quad    Mask bits indicating which quadrant we're doing
    @param  delta   Offset from center-point, used for round-rects
    @param  colour  16-bit 5-6-5 colour to fill with
*/
/**************************************************************************/
void Adafruit_GFX::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t quad, int16_t delta, uint16_t colour)
{
  int16_t f = 1-r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2*r;
  int16_t x = 0;
  int16_t y = r;
  int16_t px = x;
  int16_t py = y;

  delta++; // Avoid some +1's in the loop

  while(x < y)
  {
    if(f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    // These checks avoid double-drawing certain lines, important
    // for the SSD1306 library which has an INVERT drawing mode.
    if(x < y+1)
    {
      if(quad&0x1)
      {
        writeFastVLine(x0+x, y0-y, 2*y+delta, colour);
      }
      if(quad&0x2)
      {
        writeFastVLine(x0-x, y0-y, 2*y+delta, colour);
      }
    }
    if(y != py)
    {
      if(quad&0x1)
      {
        writeFastVLine(x0+py, y0-px, 2*px+delta, colour);
      }
      if(quad&0x2)
      {
        writeFastVLine(x0-py, y0-px, 2*px+delta, colour);
      }
      py = y;
    }
    px = x;
  }
}

/**************************************************************************/
/*!
  @brief  Draw a rectangle with no fill colour
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  w       Width in pixels
  @param  h       Height in pixels
  @param  colour  16-bit 5-6-5 colour to draw with
*/
/**************************************************************************/
void Adafruit_GFX::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colour)
{
  startWrite();
  writeFastHLine(x, y, w, colour);
  writeFastHLine(x, y+h-1, w, colour);
  writeFastVLine(x, y, h, colour);
  writeFastVLine(x+w-1, y, h, colour);
  endWrite();
}

/**************************************************************************/
/*!
  @brief  Draw a rounded rectangle with no fill colour
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  w       Width in pixels
  @param  h       Height in pixels
  @param  r       Radius of corner rounding
  @param  colour  16-bit 5-6-5 colour to draw with
*/
/**************************************************************************/
void Adafruit_GFX::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t colour)
{
  int16_t max_radius = ((w < h) ? w : h)/2; // 1/2 minor axis

  if(r > max_radius)
  {
    r = max_radius;
  }
  // smarter version
  startWrite();
  writeFastHLine(x+r, y, w-2*r, colour); // Top
  writeFastHLine(x+r, y+h-1, w-2*r, colour); // Bottom
  writeFastVLine(x, y+r, h-2*r, colour); // Left
  writeFastVLine(x+w-1, y+r, h-2*r, colour); // Right
  // draw four corners
  drawCircleHelper(x+r, y+r, r, 0x1, colour);
  drawCircleHelper(x+w-r-1, y+r, r, 0x2, colour);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 0x4, colour);
  drawCircleHelper(x+r, y+h-r-1, r, 0x8, colour);
  endWrite();
}

/**************************************************************************/
/*!
  @brief  Draw a rounded rectangle with fill colour
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  w       Width in pixels
  @param  h       Height in pixels
  @param  r       Radius of corner rounding
  @param  colour  16-bit 5-6-5 colour to draw/fill with
*/
/**************************************************************************/
void Adafruit_GFX::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t colour)
{
  int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis

  if(r > max_radius)
  {
    r = max_radius;
  }
  // smarter version
  startWrite();
  writeFillRect(x+r, y, w-2*r, h, colour);
  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 0x1, h-2*r-1, colour);
  fillCircleHelper(x+r, y+r, r, 0x2, h-2*r-1, colour);
  endWrite();
}

/**************************************************************************/
/*!
  @brief  Draw a triangle with no fill colour
  @param  x0      Vertex #0 x coordinate
  @param  y0      Vertex #0 y coordinate
  @param  x1      Vertex #1 x coordinate
  @param  y1      Vertex #1 y coordinate
  @param  x2      Vertex #2 x coordinate
  @param  y2      Vertex #2 y coordinate
  @param  colour  16-bit 5-6-5 colour to draw with
*/
/**************************************************************************/
void Adafruit_GFX::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t colour)
{
  drawLine(x0, y0, x1, y1, colour);
  drawLine(x1, y1, x2, y2, colour);
  drawLine(x2, y2, x0, y0, colour);
}

/**************************************************************************/
/*!
   @brief     Draw a triangle with colour-fill
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    colour 16-bit 5-6-5 colour to fill/draw with
*/
/**************************************************************************/
void Adafruit_GFX::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t colour)
{
  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if(y0 > y1)
  {
    swap(y0, y1);
    swap(x0, x1);
  }
  if(y1 > y2)
  {
    swap(y2, y1);
    swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1);
    swap(x0, x1);
  }

  startWrite();
  if(y0 == y2)
  { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)
    {
      a = x1;
    }
    else if(x1 > b)
    {
      b = x1;
    }
    if(x2 < a)
    {
      a = x2;
    }
    else if(x2 > b)
    {
      b = x2;
    }
    writeFastHLine(a, y0, b-a+1, colour);
    endWrite();
    return;
  }

  int16_t dx01 = x1-x0;
  int16_t dy01 = y1-y0;
  int16_t dx02 = x2-x0;
  int16_t dy02 = y2-y0;
  int16_t dx12 = x2-x1;
  int16_t dy12 = y2-y1;
  int32_t sa = 0;
  int32_t sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2)
  {
    last = y1;   // Include y1 scanline
  }
  else
  {
    last = y1-1; // Skip it
  }

  for(y = y0; y <= last; y++)
  {
    a = x0+sa/dy01;
    b = x0+sb/dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b)
    {
      swap(a,b);
    }
    writeFastHLine(a, y, b-a+1, colour);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12*(y-y1);
  sb = dx02*(y-y0);
  for(; y <= y2; y++)
  {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b)
    {
      swap(a, b);
    }
    writeFastHLine(a, y, b-a+1, colour);
  }
  endWrite();
}

// BITMAP / XBITMAP / GRAYSCALE / RGB BITMAP FUNCTIONS ---------------------

/**************************************************************************/
/*!
  @brief  Draw a 1-bit image at the specified (x,y) position, using the specified foreground colour (unset bits are transparent).
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  bitmap  byte array with monochrome bitmap
  @param  w       Width of bitmap in pixels
  @param  h       Height of bitmap in pixels
  @param  colour  16-bit 5-6-5 colour to draw with
*/
/**************************************************************************/
void Adafruit_GFX::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t colour)
{
  int16_t byteWidth = (w+7)/8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;

  startWrite();
  for(int16_t j = 0; j < h; j++, y++)
  {
    for(int16_t i = 0; i < w; i++)
    {
      if(i&7)
      {
        byte <<= 1;
      }
      else
      {
        byte = bitmap[j*byteWidth+i/8];
      }
      if(byte&0x80)
      {
        writePixel(x+i, y, colour);
      }
    }
  }
  endWrite();
}

/**************************************************************************/
/*!
  @brief  Draw a PROGMEM-resident 1-bit image at the specified (x,y) position, using the specified foreground (for set bits) and background (unset bits) colours.
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  bitmap  byte array with monochrome bitmap
  @param  w       Width of bitmap in pixels
  @param  h       Height of bitmap in pixels
  @param  colour  16-bit 5-6-5 colour to draw pixels with
  @param  bg      16-bit 5-6-5 colour to draw background with
*/
/**************************************************************************/
void Adafruit_GFX::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t colour, uint16_t bg)
{
  int16_t byteWidth = (w+7)/8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;

  startWrite();
  for(int16_t j = 0; j < h; j++, y++)
  {
    for(int16_t i = 0; i < w; i++)
    {
      if(i&7)
      {
        byte <<= 1;
      }
      else
      {
        byte = bitmap[j*byteWidth+i/8];
      }
      writePixel(x+i, y, (byte&0x80) ? colour : bg);
    }
  }
  endWrite();
}

/**************************************************************************/
/*!
  @brief  Draw a RAM-resident 1-bit image at the specified (x,y) position, using the specified foreground colour (unset bits are transparent).
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  bitmap  byte array with monochrome bitmap
  @param  w       Width of bitmap in pixels
  @param  h       Height of bitmap in pixels
  @param  colour  16-bit 5-6-5 colour to draw with
*/
/**************************************************************************/
void Adafruit_GFX::drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t colour)
{
  int16_t byteWidth = (w+7)/8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;

  startWrite();
  for(int16_t j = 0; j < h; j++, y++)
  {
    for(int16_t i = 0; i < w; i++)
    {
      if(i&7)
      {
        byte <<= 1;
      }
      else
      {
        byte = bitmap[j*byteWidth+i/8];
      }
      if(byte&0x80)
      {
        writePixel(x+i, y, colour);
      }
    }
  }
  endWrite();
}

/**************************************************************************/
/*!
  @brief  Draw 1-bit image at the specified (x,y) position, using the specified foreground (for set bits) and background (unset bits) colours.
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  bitmap  byte array with monochrome bitmap
  @param  w       Width of bitmap in pixels
  @param  h       Height of bitmap in pixels
  @param  colour  16-bit 5-6-5 colour to draw pixels with
  @param  bg      16-bit 5-6-5 colour to draw background with
*/
/**************************************************************************/
void Adafruit_GFX::drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t colour, uint16_t bg)
{
  int16_t byteWidth = (w+7)/8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;

  startWrite();
  for(int16_t j = 0; j < h; j++, y++)
  {
    for(int16_t i = 0; i < w; i++)
    {
      if(i&7)
      {
        byte <<= 1;
      }
      else
      {
        byte = bitmap[j*byteWidth+i/8];
      }
      writePixel(x+i, y, (byte&0x80) ? colour : bg);
    }
  }
  endWrite();
}

/**************************************************************************/
/*!
  @brief  Draw XBitMap Files (*.xbm), exported from GIMP. 
          Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in
          editor.  C Array can be directly used with this function.  There
          is no RAM-resident version of this function; if generating
          bitmaps in RAM, use the format defined by drawBitmap() and call
          that instead.
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  bitmap  byte array with monochrome bitmap
  @param  w       Width of bitmap in pixels
  @param  h       Height of bitmap in pixels
  @param  colour  16-bit 5-6-5 colour to draw pixels with
*/
/**************************************************************************/
void Adafruit_GFX::drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t colour)
{
  int16_t byteWidth = (w+7)/8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;

  startWrite();
  for(int16_t j = 0; j < h; j++, y++)
  {
    for(int16_t i = 0; i < w; i++)
    {
      if(i&7)
      {
        byte >>= 1;
      }
      else
      {
        byte = bitmap[j*byteWidth+i/8];
      }
      // Nearly identical to drawBitmap(), only the bit order
      // is reversed here (left-to-right = LSB to MSB):
      if(byte&0x01)
      {
        writePixel(x+i, y, colour);
      }
    }
  }
  endWrite();
}


/**************************************************************************/
/*!
  @brief  Draw a 8-bit image (grayscale) at the specified (x,y) pos.
          Specifically for 8-bit display devices such as IS31FL3731; no
          colour reduction/expansion is performed.
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  bitmap  byte-array with grayscale bitmap
  @param  w       Width of bitmap in pixels
  @param  h       Height of bitmap in pixels
*/
/**************************************************************************/
void Adafruit_GFX::drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h)
{
  startWrite();
  for(int16_t j = 0; j < h; j++, y++)
  {
    for(int16_t i = 0; i < w; i++ )
    {
      writePixel(x+i, y, (uint8_t) bitmap[j*w+i]);
    }
  }
  endWrite();
}

/**************************************************************************/
/*!
  @brief  Draw a 8-bit image (grayscale) at the specified (x,y) pos.  
          Specifically for 8-bit display devices such as IS31FL3731; no
          colour reduction/expansion is performed.
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  bitmap  byte-array with grayscale bitmap
  @param  w       Width of bitmap in pixels
  @param  h       Height of bitmap in pixels
*/
/**************************************************************************/
void Adafruit_GFX::drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h)
{
  startWrite();
  for(int16_t j = 0; j < h; j++, y++)
  {
    for(int16_t i = 0; i < w; i++)
    {
      writePixel(x+i, y, bitmap[j*w+i]);
    }
  }
  endWrite();
}


/**************************************************************************/
/*!
  @brief  Draw a 8-bit image (grayscale) with a 1-bit mask (set bits = opaque,
          unset bits = clear) at the specified (x,y) position.  BOTH
          buffers (grayscale and mask) must be PROGMEM-resident.
          Specifically for 8-bit display devices such as IS31FL3731; no
          colour reduction/expansion is performed.
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  bitmap  byte array with grayscale bitmap
  @param  mask    byte array with mask bitmap
  @param  w       Width of bitmap in pixels
  @param  h       Height of bitmap in pixels
*/
/**************************************************************************/
void Adafruit_GFX::drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], const uint8_t mask[], int16_t w, int16_t h)
{
  int16_t bw = (w+7)/8; // Bitmask scanline pad = whole byte
  uint8_t byte = 0;

  startWrite();
  for(int16_t j = 0; j < h; j++, y++)
  {
    for(int16_t i = 0; i < w; i++)
    {
      if(i&7)
      {
        byte <<= 1;
      }
      else
      {
        byte = mask[j*bw+i/8];
      }
      if(byte&0x80)
      {
        writePixel(x+i, y, (uint8_t) bitmap[j*w+i]);
      }
    }
  }
  endWrite();
}

/**************************************************************************/
/*!
  @brief  Draw a 8-bit image (grayscale) with a 1-bit mask (set bits =
          opaque, unset bits = clear) at the specified (x,y) position.
          BOTH buffers (grayscale and mask) must be RAM-residentt, no
          mix-and-match Specifically for 8-bit display devices such as
          IS31FL3731; no colour reduction/expansion is performed.
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  bitmap  byte array with grayscale bitmap
  @param  mask    byte array with mask bitmap
  @param  w       Width of bitmap in pixels
  @param  h       Height of bitmap in pixels
*/
/**************************************************************************/
void Adafruit_GFX::drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint8_t *mask, int16_t w, int16_t h)
{
  int16_t bw = (w+7)/8; // Bitmask scanline pad = whole byte
  uint8_t byte = 0;

  startWrite();
  for(int16_t j = 0; j < h; j++, y++)
  {
    for(int16_t i = 0; i < w; i++)
    {
      if(i&7)
      {
        byte <<= 1;
      }
      else
      {
        byte = mask[j*bw+i/8];
      }
      if(byte&0x80)
      {
        writePixel(x+i, y, bitmap[j*w+i]);
      }
    }
  }
  endWrite();
}


/**************************************************************************/
/*!
  @brief  Draw a 16-bit image (RGB 5/6/5) at the specified (x,y) position.
          For 16-bit display devices; no colour reduction performed.
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  bitmap  byte-array with 16-bit colour bitmap
  @param  w       Width of bitmap in pixels
  @param  h       Height of bitmap in pixels
*/
/**************************************************************************/
void Adafruit_GFX::drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h)
{
  startWrite();
  for(int16_t j = 0; j < h; j++, y++)
  {
    for(int16_t i = 0; i < w; i++)
    {
      writePixel(x+i, y, bitmap[j*w+i]);
    }
  }
  endWrite();
}

/**************************************************************************/
/*!
  @brief  Draw a 16-bit image (RGB 5/6/5) at the specified (x,y) position.  
          For 16-bit display devices; no colour reduction performed.
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  bitmap  byte-array with 16-bit colour bitmap
  @param  w       Width of bitmap in pixels
  @param  h       Height of bitmap in pixels
*/
/**************************************************************************/
void Adafruit_GFX::drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h)
{
  startWrite();
  for(int16_t j = 0; j < h; j++, y++)
  {
    for(int16_t i = 0; i < w; i++)
    {
      writePixel(x+i, y, bitmap[j*w+i]);
    }
  }
  endWrite();
}


/**************************************************************************/
/*!
  @brief  Draw a 16-bit image (RGB 5/6/5) with a 1-bit mask (set bits =
          opaque, unset bits = clear) at the specified (x,y) position. BOTH
          buffers (colour and mask) must be PROGMEM-resident. For 16-bit
          display devices; no colour reduction performed.
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  bitmap  byte array with 16-bit colour bitmap
  @param  mask    byte array with monochrome mask bitmap
  @param  w       Width of bitmap in pixels
  @param  h       Height of bitmap in pixels
*/
/**************************************************************************/
void Adafruit_GFX::drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], const uint8_t mask[], int16_t w, int16_t h)
{
  int16_t bw = (w+7)/8; // Bitmask scanline pad = whole byte
  uint8_t byte = 0;

  startWrite();
  for(int16_t j = 0; j < h; j++, y++)
  {
    for(int16_t i = 0; i < w; i++)
    {
      if(i&7)
      {
        byte <<= 1;
      }
      else
      {
        byte = mask[j*bw+i/8];
      }
      if(byte & 0x80)
      {
        writePixel(x+i, y, bitmap[j*w+i]);
      }
    }
  }
  endWrite();
}

/**************************************************************************/
/*!
  @brief  Draw a 16-bit image (RGB 5/6/5) with a 1-bit mask (set bits =
          opaque, unset bits = clear) at the specified (x,y) position. BOTH
          buffers (colour and mask) must be RAM-resident. For 16-bit
          display devices; no colour reduction performed.
  @param  x       Top-left corner x-coordinate
  @param  y       Top-left corner y-coordinate
  @param  bitmap  byte-array with 16-bit colour bitmap
  @param  mask    byte-array with monochrome mask bitmap
  @param  w       Width of bitmap in pixels
  @param  h       Height of bitmap in pixels
*/
/**************************************************************************/
void Adafruit_GFX::drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h)
{
  int16_t bw = (w+7)/8; // Bitmask scanline pad = whole byte
  uint8_t byte = 0;

  startWrite();
  for(int16_t j = 0; j < h; j++, y++)
  {
    for(int16_t i = 0; i < w; i++)
    {
      if(i&7)
      {
        byte <<= 1;
      }
      else
      {
        byte = mask[j*bw+i/8];
      }
      if(byte&0x80)
      {
        writePixel(x+i, y, bitmap[j*w+i]);
      }
    }
  }
  endWrite();
}

// TEXT- AND CHARACTER-HANDLING FUNCTIONS ----------------------------------

// Draw a character
/**************************************************************************/
/*!
  @brief  Draw a single character
  @param  x       Bottom-left corner x-coordinate
  @param  y       Bottom-left corner y-coordinate
  @param  c       The 8-bit font-indexed character (likely ascii)
  @param  colour  16-bit 5-6-5 colour to draw chraracter with
  @param  bg      16-bit 5-6-5 colour to fill background with (if same as
                  colour, no background)
  @param  size    Font magnification level, 1 is 'original' size
*/
/**************************************************************************/
void Adafruit_GFX::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t colour, uint16_t bg, uint8_t size)
{
  if(!gfxFont)
  { // 'Classic' built-in font
    if(x >= _width            ||  // Clip right
       y >= _height           ||  // Clip bottom
       (x + 6 * size - 1) < 0 ||  // Clip left
       (y + 8 * size - 1) < 0)    // Clip top
    {
      return;
    }

    if(!_cp437 && c >= 176)
    {
      c++; // Handle 'classic' charset behavior
    }

    startWrite();
    for(int8_t i = 0; i < 5; i++)
    { // Char bitmap = 5 columns
      uint8_t line = font[c*5+i];

      for(int8_t j = 0; j < 8; j++, line >>= 1)
      {
        if(line&1)
        {
          if(size == 1)
          {
            writePixel(x+i, y+j, colour);
          }
          else
          {
            writeFillRect(x+i*size, y+j*size, size, size, colour);
          }
        }
        else if(bg != colour)
        {
          if(size == 1)
          {
            writePixel(x+i, y+j, bg);
          }
          else
          {
            writeFillRect(x+i*size, y+j*size, size, size, bg);
          }
        }
      }
    }
    if(bg != colour)
    { // If opaque, draw vertical line for last column
        if(size == 1)
        {
          writeFastVLine(x+5, y, 8, bg);
        }
        else
        {
          writeFillRect(x+5*size, y, size, 8*size, bg);
        }
    }
    endWrite();

  }
  else
  { // Custom font
    // Character is assumed previously filtered by write() to eliminate
    // newlines, returns, non-printable characters, etc.  Calling
    // drawChar() directly with 'bad' characters of font may cause mayhem!

    c -= gfxFont->first;
    GFXglyph *glyph  = (GFXglyph *) &gfxFont->glyph[c];
    uint8_t  *bitmap = gfxFont->bitmap;

    uint16_t bo = glyph->bitmapOffset;
    uint8_t w = glyph->width, h = glyph->height;
    int8_t xo = glyph->xOffset, yo = glyph->yOffset;
    uint8_t xx, yy, bits = 0, bit = 0;
    int16_t xo16 = 0, yo16 = 0;

    if(size > 1)
    {
      xo16 = xo;
      yo16 = yo;
    }

    // Todo: Add character clipping here

    // NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
    // THIS IS ON PURPOSE AND BY DESIGN.  The background colour feature
    // has typically been used with the 'classic' font to overwrite old
    // screen contents with new data.  This ONLY works because the
    // characters are a uniform size; it's not a sensible thing to do with
    // proportionally-spaced fonts with glyphs of varying sizes (and that
    // may overlap).  To replace previously-drawn text when using a custom
    // font, use the getTextBounds() function to determine the smallest
    // rectangle encompassing a string, erase the area with fillRect(),
    // then draw new text.  This WILL infortunately 'blink' the text, but
    // is unavoidable.  Drawing 'background' pixels will NOT fix this,
    // only creates a new set of problems.  Have an idea to work around
    // this (a canvas object type for MCUs that can afford the RAM and
    // displays supporting setAddrWindow() and pushcolours()), but haven't
    // implemented this yet.

    startWrite();
    for(yy = 0; yy < h; yy++)
    {
      for(xx = 0; xx < w; xx++)
      {
        if(!(bit++&7))
        {
          bits = bitmap[bo++];
        }
        if(bits & 0x80)
        {
          if(size == 1)
          {
              writePixel(x+xo+xx, y+yo+yy, colour);
          }
          else
          {
            writeFillRect(x+(xo16+xx)*size, y+(yo16+yy)*size, size, size, colour);
          }
        }
        bits <<= 1;
      }
    }
    endWrite();
  } // End classic vs custom font
}

/**************************************************************************/
/*!
  @brief  Print one byte/character of data, used to support print()
  @param  c       The 8-bit ascii character to write
*/
/**************************************************************************/
size_t Adafruit_GFX::writeChar(uint8_t c)
{
  if(!gfxFont)                      // 'Classic' built-in font
  {
    if(c == '\n')                   // Newline?
    {
      cursor_x  = 0;                // Reset x to zero,
      cursor_y += textsize * 8;     // advance y one line
    }
    else if(c != '\r')              // Ignore carriage returns
    {
      if(wrap && ((cursor_x+textsize*6) > _width))
      {
        cursor_x  = 0;              // Reset x to zero,
        cursor_y += textsize * 8;   // advance y one line
      }
      drawChar(cursor_x, cursor_y, c, textcolour, textbgcolour, textsize);
      cursor_x += textsize * 6;     // Advance x one char
    }
  }
  else                              // Custom font
  {
    if(c == '\n')
    {
      cursor_x  = 0;
      cursor_y += textsize * gfxFont->yAdvance;
    }
    else if(c != '\r')
    {
      uint8_t first = gfxFont->first;

      if((c >= first) && (c <= gfxFont->last))
      {
        GFXglyph *glyph = (GFXglyph *) &gfxFont->glyph[c-first];
        uint8_t w = glyph->width, h = glyph->height;
        if((w > 0) && (h > 0))
        { // Is there an associated bitmap?
          int16_t xo = glyph->xOffset; // sic
          if(wrap && ((cursor_x+textsize*(xo+w)) > _width))
          {
            cursor_x  = 0;
            cursor_y += textsize*gfxFont->yAdvance;
          }
          drawChar(cursor_x, cursor_y, c, textcolour, textbgcolour, textsize);
        }
        cursor_x += glyph->xAdvance * textsize;
      }
    }
  }
  return 1;
}

/**************************************************************************/
/*!
  @brief  Set text cursor location
  @param  x       x-coordinate in pixels
  @param  y       y-coordinate in pixels
*/
/**************************************************************************/
void Adafruit_GFX::setCursor(int16_t x, int16_t y)
{
  cursor_x = x;
  cursor_y = y;
}

/**************************************************************************/
/*!
  @brief    Get text cursor X location
  @returns  x-coordinate in pixels
*/
/**************************************************************************/
int16_t Adafruit_GFX::getCursorX(void) const
{
  return cursor_x;
}

/**************************************************************************/
/*!
  @brief    Get text cursor Y location
  @returns  y-coordinate in pixels
*/
/**************************************************************************/
int16_t Adafruit_GFX::getCursorY(void) const
{
  return cursor_y;
}

/**************************************************************************/
/*!
  @brief  Set text 'magnification' size. Each increase in s makes 1 pixel that much bigger.
  @param  s       Desired text size. 1 is default 6x8, 2 is 12x16, 3 is 18x24, etc
*/
/**************************************************************************/
void Adafruit_GFX::setTextSize(uint8_t s)
{
  textsize = (s > 0) ? s : 1;
}

/**************************************************************************/
/*!
  @brief  Set text font colour with transparant background
  @param  c       16-bit 5-6-5 colour to draw text with
*/
/**************************************************************************/
void Adafruit_GFX::setTextColour(uint16_t c)
{
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  textcolour = textbgcolour = c;
}

/**************************************************************************/
/*!
  @brief  Set text font colour with custom background colour
  @param  c       16-bit 5-6-5 colour to draw text with
  @param  b       16-bit 5-6-5 colour to draw background/fill with
*/
/**************************************************************************/
void Adafruit_GFX::setTextColour(uint16_t c, uint16_t b)
{
  textcolour   = c;
  textbgcolour = b;
}

/**************************************************************************/
/*!
  @brief      Whether text that is too long should 'wrap' around to the next line.
  @param  w Set true for wrapping, false for clipping
*/
/**************************************************************************/
void Adafruit_GFX::setTextWrap(bool w)
{
  wrap = w;
}

/**************************************************************************/
/*!
  @brief      Get rotation setting for display
  @returns    0 thru 3 corresponding to 4 cardinal rotations
*/
/**************************************************************************/
uint8_t Adafruit_GFX::getRotation(void) const
{
  return rotation;
}

/**************************************************************************/
/*!
  @brief  Set rotation setting for display
  @param  x       0 thru 3 corresponding to 4 cardinal rotations
*/
/**************************************************************************/
void Adafruit_GFX::setRotation(uint8_t rot)
{
  rotation = (rot&3);
  switch(rotation)
  {
    case 0:
    case 2:
      _width  = WIDTH;
      _height = HEIGHT;
      break;
    case 1:
    case 3:
      _width  = HEIGHT;
      _height = WIDTH;
      break;
  }
}

/**************************************************************************/
/*!
  @brief  Enable (or disable) Code Page 437-compatible charset.
          There was an error in glcdfont.c for the longest time -- one
          character (#176, the 'light shade' block) was missing -- this
          threw off the index of every character that followed it.  But a
          TON of code has been written with the erroneous character
          indices.  By default, the library uses the original 'wrong'
          behavior and old sketches will still work.  Pass 'true' to this
          function to use correct CP437 character values in your code.
  @param  x       Whether to enable (True) or not (False)
*/
/**************************************************************************/
void Adafruit_GFX::cp437(bool x)
{
  _cp437 = x;
}

/**************************************************************************/
/*!
  @brief  Set the font to display when print()ing, either custom or default
  @param  f       The GFXfont object, if NULL use built in 6x8 font
*/
/**************************************************************************/
void Adafruit_GFX::setFont(const GFXfont *f)
{
  if(f)
  {            // Font struct pointer passed in?
    if(!gfxFont)
    { // And no current font struct?
        // Switching from classic to new font behavior.
        // Move cursor pos down 6 pixels so it's on baseline.
        cursor_y += 6;
    }
  }
  else if(gfxFont)
  { // NULL passed.  Current font struct defined?
    // Switching from new to classic font behavior.
    // Move cursor pos up 6 pixels so it's at top-left of char.
    cursor_y -= 6;
  }
  gfxFont = (GFXfont *) f;
}


/**************************************************************************/
/*!
  @brief  Helper to determine size of a character with current font/size.
          Broke this out as it's used by both the PROGMEM- and RAM-resident
          getTextBounds() functions.
  @param  c       The ascii character in question
  @param  x       Pointer to x location of character
  @param  y       Pointer to y location of character
  @param  minx    Minimum clipping value for X
  @param  miny    Minimum clipping value for Y
  @param  maxx    Maximum clipping value for X
  @param  maxy    Maximum clipping value for Y
*/
/**************************************************************************/
void Adafruit_GFX::charBounds(char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy)
{
  if(gfxFont)
  {
    if(c == '\n')
    { // Newline?
      *x  = 0;    // Reset x to zero, advance y by one line
      *y += textsize * gfxFont->yAdvance;
    }
    else if(c != '\r')
    { // Not a carriage return; is normal char
      uint8_t first = gfxFont->first,
              last  = gfxFont->last;

      if((c >= first) && (c <= last))
      { // Char present in this font?
        GFXglyph *glyph = (GFXglyph *) &gfxFont->glyph[c-first];
        uint8_t gw = glyph->width,
                gh = glyph->height,
                xa = glyph->xAdvance;
        int8_t  xo = glyph->xOffset,
                yo = glyph->yOffset;
        if(wrap && ((*x+(((int16_t)xo+gw)*textsize)) > _width))
        {
            *x  = 0; // Reset x to zero, advance y by one line
            *y += textsize * gfxFont->yAdvance;
        }
        int16_t ts = textsize,
                x1 = *x+xo*ts,
                y1 = *y+yo*ts,
                x2 = x1+gw*ts-1,
                y2 = y1+gh*ts-1;
        if(x1 < *minx) *minx = x1;
        if(y1 < *miny) *miny = y1;
        if(x2 > *maxx) *maxx = x2;
        if(y2 > *maxy) *maxy = y2;
        *x += xa*ts;
      }
    }
  }
  else
  { // Default font
    if(c == '\n')
    {                     // Newline?
      *x  = 0;                        // Reset x to zero,
      *y += textsize*8;             // advance y one line
      // min/max x/y unchaged -- that waits for next 'normal' character
    }
    else if(c != '\r')
    {  // Normal char; ignore carriage returns
      if(wrap && ((*x+textsize*6) > _width))
      { // Off right?
        *x  = 0;                    // Reset x to zero,
        *y += textsize*8;         // advance y one line
      }
      int x2 = *x+textsize*6-1, // Lower-right pixel of char
          y2 = *y+textsize*8-1;

      if(x2 > *maxx)
      {
        *maxx = x2;      // Track max x, y
      }
      if(y2 > *maxy)
      {
        *maxy = y2;
      }
      if(*x < *minx)
      {
        *minx = *x;      // Track min x, y
      }
      if(*y < *miny)
      {
        *miny = *y;
      }
      *x += textsize*6;             // Advance x one char
    }
  }
}

/**************************************************************************/
/*!
  @brief  Helper to determine size of a string with current font/size. Pass
          string and a cursor position, returns UL corner and W,H.
  @param  str     The ascii string to measure
  @param  x       The current cursor X
  @param  y       The current cursor Y
  @param  x1      The boundary X coordinate, set by function
  @param  y1      The boundary Y coordinate, set by function
  @param  w       The boundary width, set by function
  @param  h       The boundary height, set by function
*/
/**************************************************************************/
void Adafruit_GFX::getTextBounds(const char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h)
{
  uint8_t c; // Current character

  *x1 = x;
  *y1 = y;
  *w  = *h = 0;

  int16_t minx = _width, miny = _height, maxx = -1, maxy = -1;

  while((c = *str++))
  {
    charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);
  }

  if(maxx >= minx)
  {
    *x1 = minx;
    *w  = maxx-minx+1;
  }
  if(maxy >= miny)
  {
    *y1 = miny;
    *h  = maxy-miny+1;
  }
}

/**************************************************************************/
/*!
  @brief    Get width of the display, accounting for the current rotation
  @returns  Width in pixels
*/
/**************************************************************************/
int16_t Adafruit_GFX::width(void) const
{
  return _width;
}

/**************************************************************************/
/*!
  @brief    Get height of the display, accounting for the current rotation
  @returns  Height in pixels
*/
/**************************************************************************/
int16_t Adafruit_GFX::height(void) const
{
  return _height;
}

/**************************************************************************/
/*!
  @brief  Invert the display (ideally using built-in hardware command)
  @param  i       True if you want to invert, false to make 'normal'
*/
/**************************************************************************/
void Adafruit_GFX::invertDisplay(bool i)
{
  // Do nothing, must be subclassed if supported by hardware
}

