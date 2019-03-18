#if !defined(__ADAFRUIT_SSD1306_H__)
#define __ADAFRUIT_SSD1306_H__

#include "mbed.h"
#include "Adafruit_GFX.h"

#define BLACK                           0
#define WHITE                           1
#define INVERSE                         2

extern Serial myPC;

class Adafruit_SSD1306 : public Adafruit_GFX
{
public:
  Adafruit_SSD1306(PinName rst, uint8_t w = 128, uint8_t h = 32)
    : Adafruit_GFX(w, h), _rst(rst, false)
  {
    _frmbufsz = ((w+7)/8)*h;
    _frmbuf = (uint8_t *) malloc(_frmbufsz);
  };
  ~Adafruit_SSD1306(void)
  {
    free(_frmbuf);
  }

  void begin(bool extvcc = false);
  
  virtual void command(uint8_t c) = 0;
  virtual void data(uint8_t d) = 0;

  void display(void);
  void clearDisplay(void);
  void invertDisplay(bool i);
  void dim(bool dim);
  void drawPixel(int16_t x, int16_t y, uint16_t colr);
  virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t colr);
  virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t colr);
  void startscrollright(uint8_t start, uint8_t stop);
  void startscrollleft(uint8_t start, uint8_t stop);
  void startscrolldiagright(uint8_t start, uint8_t stop);
  void startscrolldiagleft(uint8_t start, uint8_t stop);
  void stopscroll(void);
  bool getPixel(int16_t x, int16_t y);
  uint8_t *getBuffer(void);

protected:
  void drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t colr);
  void drawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t colr);
  virtual void sendDisplayBuffer() = 0;

  DigitalOut _rst;
  uint8_t _maxcontrast;

  uint8_t *_frmbuf;
  uint16_t _frmbufsz;
};

class Adafruit_SSD1306_Spi : public Adafruit_SSD1306
{
public:
  Adafruit_SSD1306_Spi(SPI &spi, PinName dc, PinName rst, PinName cs, uint8_t w = 128, uint8_t h = 32) : Adafruit_SSD1306(rst, w, h), _cs(cs,true), _dc(dc,false), _spi(spi)
  {
    begin();
    clearDisplay();
    display();
  };

  virtual void command(uint8_t c)
  {
    _cs = 1;
    _dc = 0;
    _cs = 0;
    _spi.write(c);
    _cs = 1;
  };

  virtual void data(uint8_t c)
  {
    _cs = 1;
    _dc = 1;
    _cs = 0;
    _spi.write(c);
    _cs = 1;
  };

protected:
  virtual void sendDisplayBuffer(void)
  {
    _cs = 1;
    _dc = 1;
    _cs = 0;

    for(uint16_t i=0, q=_frmbufsz; i<q; i++)
    {
      // myPC.printf("0x%X,", _frmbuf[i]);
      _spi.write(_frmbuf[i]);
    }

    if(height() == 32)
    {
      for(uint16_t i=0, q=_frmbufsz; i<q; i++)
      {
        _spi.write(0);
      }
    }

    _cs = 1;
  };

  DigitalOut _cs, _dc;
  SPI &_spi;
};

// Ref: Section 8.1.5 (MCU I2C Interface)
#define SSD1306_I2C_ADDR                    0x78
class Adafruit_SSD1306_I2c : public Adafruit_SSD1306
{
public:
  Adafruit_SSD1306_I2c(I2C &i2c, PinName rst, uint8_t i2caddr = SSD1306_I2C_ADDR, uint8_t w = 128, uint8_t h = 32) : Adafruit_SSD1306(rst, w, h), _i2c(i2c), _i2caddr(i2caddr)
      {
        begin();
        clearDisplay();
        display();
      };

  virtual void command(uint8_t c)
  {
    char buf[2] = {0x00, c};
    _i2c.write(_i2caddr, buf, sizeof(buf));
  }

  virtual void data(uint8_t d)
  {
    char buf[2] = {0x40, d};
    _i2c.write(_i2caddr, buf, sizeof(buf));
  };

protected:
  virtual void sendDisplayBuffer()
  {
    char buf[17];
    buf[0] = 0x40;

    for(uint16_t i=0; i<_frmbufsz; i+=16) 
    {
      memcpy(&buf[1], &_frmbuf[i], (_frmbufsz-i)<16?(_frmbufsz-i):16);
      _i2c.write(_i2caddr, buf, sizeof(buf));
    }
  };

  I2C &_i2c;
  uint8_t _i2caddr;
};

#endif // !defined(__ADAFRUIT_SSD1306_H__)

