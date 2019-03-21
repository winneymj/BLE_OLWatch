#include "mbed.h"
#include "Adafruit_SSD1306.h"

// Ref: Table 9-1, Fundamental Command Table
#define CMD_SETCNTR                 (0x81)
#define CMD_ALLDISPON(res)              (0xA4|((res)?0x0:0x1))
#define CMD_SETNORMINVDISP(inv)         (0xA6|((inv)?0x1:0x0))
#define CMD_SETDISPONOFF(on)            (0xAE|((on)?0x1:00))

// Ref: Table 9-1, Scrolling Command Table
#define CMD_CONTHORZSCRL(left)          (0x26|((left)?0x1:0x0))
#define CMD_CONTVERTHORZSCRL(left)      (0x28|((left)?0x2:0x1))
#define CMD_DEACTSCRL                   (0x2E)
#define CMD_ACTSCRL                     (0x2F)
#define CMD_SETVERTSCRLAREA             (0xA3)

// Ref: Table 9-1, Addressing Setting Command Table
#define CMD_SETLOWCOLUMN(x)             (0x00|((x)&0x0F))
#define CMD_SETHIGHCOLUMN(x)            (0x10|((x)&0x0F))
#define CMD_SETMEMADDRMODE              (0x20)
#define CMD_SETCOLADDR                  (0x21)
#define CMD_SETPAGEADDR                 (0x22)

// Ref: Table 9-1, Hardware Configuration Command Table
#define CMD_SETSTARTLINE(x)             (0x40|((x)&0x3F))
#define CMD_SEGREMAP(hi)                (0xA0|((hi)?0x1:0x00))
#define CMD_SETMUXRATIO                 (0xA8)
#define CMD_SETCOMOUTSCANDIR(fwd)       (0xC0|((fwd)?0x0:0x8))
#define CMD_SETDISPOFS                  (0xD3)
#define CMD_SETCOMPINSHWCFG             (0xDA)

// Ref: Table 9-1, Timing & Driving Scheme Setting Command Table
#define CMD_SETDISPLAYCLOCKDIV          (0xD5)
#define CMD_SETPRECHARGE                (0xD9)
#define CMD_SETVCOMDETECT               (0xDB)
#define CMD_NOP                         (0xE3)

// Ref: Section 2.1 (Command Table for Charge Bump Setting)
#define CMD_CHARGEPUMP                  (0x8D)

extern Serial pc;

void Adafruit_SSD1306::begin(bool extvcc)
{
  _rst = 1;
  wait_ms(1);
  _rst = 0;
  wait_ms(10);
  _rst = 1;

  command(CMD_SETDISPONOFF(false));

  command(CMD_SETDISPLAYCLOCKDIV);
  command(0x80);

  command(CMD_SETMUXRATIO);
  command(HEIGHT-1);

  command(CMD_SETDISPOFS);
  command(0x0);

  command(CMD_SETSTARTLINE(0x0));

  command(CMD_CHARGEPUMP);
  command(extvcc?0x10:0x14);

  command(CMD_SETMEMADDRMODE);
  command(0x00);

  command(CMD_SEGREMAP(true));
  command(CMD_SETCOMOUTSCANDIR(false));

  _maxcontrast = 0x8F;
  if(128 == WIDTH && 32 == HEIGHT)
  {
    command(CMD_SETCOMPINSHWCFG);
    command(0x02);
  }
  else if(128 == WIDTH && 64 == HEIGHT)
  {
    command(CMD_SETCOMPINSHWCFG);
    command(0x12);

    _maxcontrast = extvcc?0x9F:0xCF;
  }
  else if(96 == WIDTH && 16 == HEIGHT)
  {
    command(CMD_SETCOMPINSHWCFG);
    command(0x02);

    _maxcontrast = (extvcc?0x10:0xAF);
  }
  command(CMD_SETCNTR);
  command(_maxcontrast);

  command(CMD_SETPRECHARGE);
  command(extvcc?0x22:0xF1);

  command(CMD_SETVCOMDETECT);
  command(0x40);

  command(CMD_ALLDISPON(true));

  command(CMD_SETNORMINVDISP(false));
  
  command(CMD_SETDISPONOFF(true));
}

void Adafruit_SSD1306::drawPixel(int16_t x, int16_t y, uint16_t colr)
{
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
  {
    return;
  }
  
  switch(getRotation())
  {
    case 1:
      swap(x, y);
      x = WIDTH-x-1;
      break;
    case 2:
      x = WIDTH-x-1;
      y = HEIGHT-y-1;
      break;
    case 3:
      swap(x, y);
      y = HEIGHT-y-1;
      break;
  }
  switch(colr)
  {
    case WHITE:
      _frmbuf[x+(y/8)*WIDTH] |= (1<<(y&7));
      break;
    case BLACK:
      _frmbuf[x+(y/8)*WIDTH] &= ~(1<<(y&7));
      break;
    case INVERSE:
      _frmbuf[x+(y/8)*WIDTH] ^= (1<<(y&7));
      break;
  }
}

void Adafruit_SSD1306::clearDisplay(void)
{
  memset(_frmbuf, 0, _frmbufsz);
}

void Adafruit_SSD1306::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t colr)
{
  bool bSwap = false;
  switch(rotation) {
   case 1:
    bSwap = true;
    swap(x, y);
    x = WIDTH-x-1;
    break;
   case 2:
    x = WIDTH-x-1;
    y = HEIGHT-y-1;
    x -= (w-1);
    break;
   case 3:
    bSwap = true;
    swap(x, y);
    y = HEIGHT-y-1;
    y -= (w-1);
    break;
  }

  if(bSwap)
  {
    drawFastVLineInternal(x, y, w, colr);
  }
  else
  {
    drawFastHLineInternal(x, y, w, colr);
  }
}

void Adafruit_SSD1306::drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t colr)
{
  if((y >= 0) && (y < HEIGHT))
  {
    if(x < 0)
    {
      w += x;
      x = 0;
    }
    if((x + w) > WIDTH)
    {
      w = WIDTH-x;
    }
    if(w > 0)
    {
      uint16_t i = (y/8)*WIDTH+x;
      uint8_t mask = 1<<(y&7);

      switch(colr)
      {
        case WHITE:
          while(w--)
          {
            _frmbuf[i++] |= mask;
          }
          break;
        case BLACK:
          mask = ~mask;
          while(w--)
          {
            _frmbuf[i++] &= mask;
          }
          break;
        case INVERSE:
          while(w--)
          {
            _frmbuf[i++] ^= mask;
          }
          break;
      }
    }
  }
}

void Adafruit_SSD1306::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t colr)
{
  bool bSwap = false;
  switch(rotation)
  {
    case 1:
      bSwap = true;
      swap(x, y);
      x = WIDTH-x-1;
      x -= (h-1);
      break;
    case 2:
      x = WIDTH-x-1;
      y = HEIGHT-y-1;
      y -= (h-1);
      break;
    case 3:
      bSwap = true;
      swap(x, y);
      y = HEIGHT-y-1;
      break;
  }

  if(bSwap)
  {
    drawFastHLineInternal(x, y, h, colr);
  }
  else
  {
    drawFastVLineInternal(x, y, h, colr);
  }
}

void Adafruit_SSD1306::drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, uint16_t colr)
{
  // do nothing if we're off the left or right side of the screen
  if(x < 0 || x >= WIDTH) { return; }

  // make sure we don't try to draw below 0
  if(__y < 0)
  {
    __h += __y;
    __y = 0;
  }

  // make sure we don't go past the height of the display
  if((__y + __h) > HEIGHT)
  {
    __h = (HEIGHT - __y);
  }

  // if our height is now negative, punt
  if(__h <= 0) {
    return;
  }

  // this display doesn't need ints for coordinates, use local byte registers for faster juggling
  register uint8_t  y = __y;
  register uint8_t  h = __h;

  // set up the pointer for fast movement through the buffer
  register uint8_t *pBuf = _frmbuf;
  // adjust the buffer pointer for the current row
  pBuf += ((y/8) * WIDTH);
  // and offset x columns in
  pBuf += x;

  // uint8_t i = (y/8)*WIDTH+x;

  // do the first partial byte, if necessary - this requires some masking
  register uint8_t mod = (y&7);
  if(mod)
  {
    // mask off the high n bits we want to set
    mod = 8-mod;
    static const uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE};
    uint8_t mask = premask[mod];

    // adjust the mask if we're not going to reach the end of this byte
    if(h < mod)
    {
      mask &= (0XFF >> (mod-h));
    }

    switch(colr)
    {
      case WHITE:   *pBuf |=  mask;  break;
      case BLACK:   *pBuf &= ~mask;  break;
      case INVERSE: *pBuf ^=  mask;  break;
    }

    // fast exit if we're done here!
    if(h < mod) { return; }

    h -= mod;

    pBuf += WIDTH;
  }

  // write solid bytes while we can - effectively doing 8 rows at a time
  if(h >= 8)
  {
    if(colr == INVERSE) // write solid bytes while we can - effectively doing 8 rows at a time
    {
      do
      {
        *pBuf=~(*pBuf);
        // adjust the buffer forward 8 rows worth of data
        pBuf += WIDTH;

        // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
        h -= 8;
      } while(h >= 8);
    }
    else
    {
      // store a local value to work with
      uint8_t val = (colr != BLACK) ? 255 : 0;

      do
      {
        // write our value in
        *pBuf = val;

        // adjust the buffer forward 8 rows worth of data
        pBuf += WIDTH;

        // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
        h -= 8;
      } while(h >= 8);
    }
  }

  // now do the final partial byte, if necessary
  if(h)
  {
    mod = h & 7;
    // this time we want to mask the low bits of the byte, vs the high bits we did above
    // register uint8_t mask = (1 << mod) - 1;
    // note - lookup table results in a nearly 10% performance improvement in fill* functions
    static const uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F};
    uint8_t mask = postmask[mod];

    switch(colr)
    {
      case WHITE:   *pBuf |=  mask;  break;
      case BLACK:   *pBuf &= ~mask;  break;
      case INVERSE: *pBuf ^=  mask;  break;
    }
  }
}

bool Adafruit_SSD1306::getPixel(int16_t x, int16_t y) {
  if((x >= 0) && (x < width()) && (y >= 0) && (y < height()))
  {
    switch(getRotation())
    {
      case 1:
        swap(x, y);
        x = WIDTH-x-1;
        break;
      case 2:
        x = WIDTH-x-1;
        y = HEIGHT-y-1;
        break;
      case 3:
        swap(x, y);
        y = HEIGHT-y-1;
        break;
    }
    return _frmbuf[x+(y/8)*WIDTH]&(1<<(y&7));
  }
  return false;
}

uint8_t *Adafruit_SSD1306::getBuffer(void)
{
  return _frmbuf;
}

void Adafruit_SSD1306::display(void)
{
  command(CMD_SETPAGEADDR);
  command(0x00);
  command(0xFF);

  command(CMD_SETCOLADDR);
  command(0x00);
  command(WIDTH-1);

  sendDisplayBuffer();
}

void Adafruit_SSD1306::startscrollright(uint8_t start, uint8_t stop)
{
  command(CMD_CONTHORZSCRL(false));
  command(0x00);
  command(start);
  command(0X00);
  command(stop);
  command(0x00);
  command(0xFF);
  command(CMD_ACTSCRL);
}

void Adafruit_SSD1306::startscrollleft(uint8_t start, uint8_t stop)
{
  command(CMD_CONTHORZSCRL(true));
  command(0x00);
  command(start);
  command(0X00);
  command(stop);
  command(0x00);
  command(0xFF);
  command(CMD_ACTSCRL);
}

void Adafruit_SSD1306::startscrolldiagright(uint8_t start, uint8_t stop)
{
  command(CMD_SETVERTSCRLAREA);
  command(0x00);
  command(HEIGHT);
  command(CMD_CONTVERTHORZSCRL(false));
  command(0x00);
  command(start);
  command(0X00);
  command(stop);
  command(0X01);
  command(CMD_ACTSCRL);
}

void Adafruit_SSD1306::startscrolldiagleft(uint8_t start, uint8_t stop)
{
  command(CMD_SETVERTSCRLAREA);
  command(0x00);
  command(HEIGHT);
  command(CMD_CONTVERTHORZSCRL(true));
  command(0x00);
  command(start);
  command(0X00);
  command(stop);
  command(0X01);
  command(CMD_ACTSCRL);
}

void Adafruit_SSD1306::stopscroll(void)
{
  command(CMD_DEACTSCRL);
}

void Adafruit_SSD1306::invertDisplay(bool i)
{
  command(CMD_SETNORMINVDISP(i));
}

void Adafruit_SSD1306::dim(bool dim)
{
  uint8_t contrast;

  if(dim)
  {
    contrast = 0;
  }
  else
  {
    contrast = _maxcontrast;
  }
  command(CMD_SETCNTR);
  command(contrast);
}

