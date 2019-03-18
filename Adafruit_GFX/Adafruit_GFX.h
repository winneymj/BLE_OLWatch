#if !defined(__ADAFRUIT_GFX_H__)
#define __ADAFRUIT_GFX_H__

#include "gfxfont.h"

static inline void swap(int16_t &a, int16_t &b)
{
    int16_t t = a;
    
    a = b;
    b = t;
}

class Adafruit_GFX : public Stream
{
public:
  Adafruit_GFX(int16_t w, int16_t h);

  virtual int _putc(int v);
  virtual int _getc(void);

  // CONTROL API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  virtual void setRotation(uint8_t rot);
  virtual void invertDisplay(bool i);

  // TRANSACTION API / CORE DRAW API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  virtual void startWrite(void);
  // This MUST be defined by the subclass:
  virtual void drawPixel(int16_t x, int16_t y, uint16_t colr) = 0;    ///< Virtual drawPixel() function to draw to the screen/framebuffer/etc, must be overridden in subclass. @param x X coordinate.  @param y Y coordinate. @param colr 16-bit pixel colr. 
  virtual void writePixel(int16_t x, int16_t y, uint16_t colr);
  virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colr);
  virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t colr);
  virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t colr);
  virtual void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colr);
  virtual void endWrite(void);

  // BASIC DRAW API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  // It's good to implement those, even if using transaction API
  virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t colr);
  virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t colr);
  virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colr);
  virtual void fillScreen(uint16_t colr);
  // Optional and probably not necessary to change
  virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colr);
  virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colr);

  // These exist only with Adafruit_GFX (no subclass overrides)
  void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t colr);
  void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t quad, uint16_t colr);
  void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t colr);
  void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t quad, int16_t delta, uint16_t colr);
  void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t colr);
  void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t colr);
  void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t colr);
  void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t colr);
  void drawBitmap(int16_t x, int16_t y, const uint8_t bmp[], int16_t w, int16_t h, uint16_t colr);
  void drawBitmap(int16_t x, int16_t y, const uint8_t bmp[], int16_t w, int16_t h, uint16_t colr, uint16_t bg);
  void drawBitmap(int16_t x, int16_t y, uint8_t *bmp, int16_t w, int16_t h, uint16_t colr);
  void drawBitmap(int16_t x, int16_t y, uint8_t *bmp, int16_t w, int16_t h, uint16_t colr, uint16_t bg);
  void drawXBitmap(int16_t x, int16_t y, const uint8_t bmp[], int16_t w, int16_t h, uint16_t colr);
  void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bmp[], int16_t w, int16_t h);
  void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bmp, int16_t w, int16_t h);
  void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bmp[], const uint8_t mask[], int16_t w, int16_t h);
  void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bmp, uint8_t *mask, int16_t w, int16_t h);
  void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bmp[], int16_t w, int16_t h);
  void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bmp, int16_t w, int16_t h);
  void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bmp[], const uint8_t mask[], int16_t w, int16_t h), drawRGBBitmap(int16_t x, int16_t y, uint16_t *bmp, uint8_t *mask, int16_t w, int16_t h);
  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t colr, uint16_t bg, uint8_t size);
  void setCursor(int16_t x, int16_t y);
  void setTextColour(uint16_t c);
  void setTextColour(uint16_t c, uint16_t bg);
  void setTextSize(uint8_t s);
  void setTextWrap(bool w);
  void cp437(bool x=true);
  void setFont(const GFXfont *f = NULL);
  void getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);

  virtual size_t writeChar(uint8_t);

  int16_t height(void) const;
  int16_t width(void) const;

  uint8_t getRotation(void) const;

  // get current cursor position (get rotation safe maximum values, using: width() for x, height() for y)
  int16_t getCursorX(void) const;
  int16_t getCursorY(void) const;

protected:
  void charBounds(char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);
  const int16_t WIDTH;  ///< This is the 'raw' display width - never changes
  const int16_t HEIGHT; ///< This is the 'raw' display height - never changes
  int16_t _width;       ///< Display width as modified by current rotation
  int16_t _height;      ///< Display height as modified by current rotation
  int16_t cursor_x;     ///< x location to start print()ing text
  int16_t cursor_y;     ///< y location to start print()ing text
  uint16_t textcolour;   ///< 16-bit background colr for print()
  uint16_t textbgcolour; ///< 16-bit text colr for print()
  uint8_t textsize;     ///< Desired magnification of text to print()
  uint8_t rotation;     ///< Display rotation (0 thru 3)
  bool wrap;            ///< If set, 'wrap' text at right edge of display
  bool _cp437;          ///< If set, use correct CP437 charset (default is off)
  GFXfont *gfxFont;     ///< Pointer to special font
};

#endif // __ADAFRUIT_GFX_H__

