/***************************************************
  This is a library for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
  as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "mbed.h"
#include "Adafruit_ST7735.h"
#include <stdio.h>

// Constructor 
Adafruit_ST7735::Adafruit_ST7735(PinName mosi, PinName miso, PinName sck, PinName cs, PinName rs, PinName rst) 
        : lcdPort(mosi, miso, sck), _cs(cs), _rs(rs), _rst(rst), Adafruit_GFX(ST7735_TFTWIDTH, ST7735_TFTHEIGHT) 
{ }

Serial pc(USBTX, USBRX);

void Adafruit_ST7735::writecommand(uint8_t c)
{
    _rs = 0;
    _cs = 0;
    lcdPort.write( c );
    _cs = 1;
}


void Adafruit_ST7735::writedata(uint8_t c)
{
    _rs = 1;
    _cs = 0;
    lcdPort.write( c );

    _cs = 1;
}


// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY 0x80
static unsigned char
Bcmd[] = {                  // Initialization commands for 7735B screens
    18,                       // 18 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
    50,                     //     50 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
    255,                    //     255 = 500 ms delay
    ST7735_COLMOD , 1+DELAY,  //  3: Set color mode, 1 arg + delay:
    0x05,                   //     16-bit color
    10,                     //     10 ms delay
    ST7735_FRMCTR1, 3+DELAY,  //  4: Frame rate control, 3 args + delay:
    0x00,                   //     fastest refresh
    0x06,                   //     6 lines front porch
    0x03,                   //     3 lines back porch
    10,                     //     10 ms delay
    ST7735_MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
    0x08,                   //     Row addr/col addr, bottom to top refresh
    ST7735_DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
    0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
    //     rise, 3 cycle osc equalize
    0x02,                   //     Fix on VTL
    ST7735_INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
    0x0,                    //     Line inversion
    ST7735_PWCTR1 , 2+DELAY,  //  8: Power control, 2 args + delay:
    0x02,                   //     GVDD = 4.7V
    0x70,                   //     1.0uA
    10,                     //     10 ms delay
    ST7735_PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
    0x05,                   //     VGH = 14.7V, VGL = -7.35V
    ST7735_PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
    0x01,                   //     Opamp current small
    0x02,                   //     Boost frequency
    ST7735_VMCTR1 , 2+DELAY,  // 11: Power control, 2 args + delay:
    0x3C,                   //     VCOMH = 4V
    0x38,                   //     VCOML = -1.1V
    10,                     //     10 ms delay
    ST7735_PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
    0x11, 0x15,
    ST7735_GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
    0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
    0x21, 0x1B, 0x13, 0x19, //      these config values represent)
    0x17, 0x15, 0x1E, 0x2B,
    0x04, 0x05, 0x02, 0x0E,
    ST7735_GMCTRN1,16+DELAY,  // 14: Sparkles and rainbows, 16 args + delay:
    0x0B, 0x14, 0x08, 0x1E, //     (ditto)
    0x22, 0x1D, 0x18, 0x1E,
    0x1B, 0x1A, 0x24, 0x2B,
    0x06, 0x06, 0x02, 0x0F,
    10,                     //     10 ms delay
    ST7735_CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
    0x00, 0x02,             //     XSTART = 2
    0x00, 0x81,             //     XEND = 129
    ST7735_RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
    0x00, 0x02,             //     XSTART = 1
    0x00, 0x81,             //     XEND = 160
    ST7735_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
    10,                     //     10 ms delay
    ST7735_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
    255
},                  //     255 = 500 ms delay

Rcmd1[] = {                 // Init for 7735R, part 1 (red or green tab)
    15,                       // 16 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
    150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
    255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
    0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
    0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
    0x01, 0x2C, 0x2D,       //     Dot inversion mode
    0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
    0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
    0xA2,
    0x02,                   //     -4.6V
    0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
    0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
    0x0A,                   //     Opamp current small
    0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
    0x8A,                   //     BCLK/2, Opamp current small & Medium low
    0x2A,
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
    0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
    0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
    0xC0,                   //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
    0x05
},                 //     16-bit color

Rcmd2green[] = {            // Init for 7735R, part 2 (green tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
    0x00, 0x02,             //     XSTART = 0
    0x00, 0x7F+0x02,        //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
    0x00, 0x01,             //     XSTART = 0
    0x00, 0x9F+0x01
},      //     XEND = 159
Rcmd2red[] = {              // Init for 7735R, part 2 (red tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
    0x00, 0x00,             //     XSTART = 0
    0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
    0x00, 0x00,             //     XSTART = 0
    0x00, 0x9F
},           //     XEND = 159

Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
    0x02, 0x1c, 0x07, 0x12,
    0x37, 0x32, 0x29, 0x2d,
    0x29, 0x25, 0x2B, 0x39,
    0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
    0x03, 0x1d, 0x07, 0x06,
    0x2E, 0x2C, 0x29, 0x2D,
    0x2E, 0x2E, 0x37, 0x3F,
    0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
    10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
    100
};                  //     100 ms delay


// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in byte array.
void Adafruit_ST7735::commandList(uint8_t *addr)
{

    uint8_t  numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;   // Number of commands to follow
    while(numCommands--) {                 // For each command...
        writecommand(*addr++); //   Read, issue command
        numArgs  = *addr++;    //   Number of args to follow
        ms       = numArgs & DELAY;          //   If hibit set, delay follows args
        numArgs &= ~DELAY;                   //   Mask out delay bit
        while(numArgs--) {                   //   For each argument...
            writedata(*addr++);  //     Read, issue argument
        }

        if(ms) {
            ms = *addr++; // Read post-command delay time (ms)
            if(ms == 255) ms = 500;     // If 255, delay for 500 ms
            wait_ms(ms);
        }
    }
}


// Initialization code common to both 'B' and 'R' type displays
void Adafruit_ST7735::commonInit(uint8_t *cmdList)
{

    colstart  = rowstart = 0; // May be overridden in init func

    _rs = 1;
    _cs = 1;

    // use default SPI format
    lcdPort.format(8,0);
    lcdPort.frequency(4000000);     // Lets try 4MHz

    // toggle RST low to reset; CS low so it'll listen to us
    _cs = 0;
    _rst = 1;
    wait_ms(500);
    _rst = 0;
    wait_ms(500);
    _rst = 1;
    wait_ms(500);

    if(cmdList) commandList(cmdList);
}


// Initialization for ST7735B screens
void Adafruit_ST7735::initB(void)
{
    commonInit(Bcmd);
}


// Initialization for ST7735R screens (green or red tabs)
void Adafruit_ST7735::initR(uint8_t options)
{
    commonInit(Rcmd1);
    if(options == INITR_GREENTAB) {
        commandList(Rcmd2green);
        colstart = 2;
        rowstart = 1;
    } else {
        // colstart, rowstart left at default '0' values
        commandList(Rcmd2red);
    }
    commandList(Rcmd3);
}


void Adafruit_ST7735::setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1,
                                    uint8_t y1)
{

    writecommand(ST7735_CASET); // Column addr set
    writedata(0x00);
    writedata(x0+colstart);     // XSTART
    writedata(0x00);
    writedata(x1+colstart);     // XEND

    writecommand(ST7735_RASET); // Row addr set
    writedata(0x00);
    writedata(y0+rowstart);     // YSTART
    writedata(0x00);
    writedata(y1+rowstart);     // YEND

    writecommand(ST7735_RAMWR); // write to RAM
}


void Adafruit_ST7735::fillScreen(uint16_t color)
{

    uint8_t x, y, hi = color >> 8, lo = color;

    setAddrWindow(0, 0, _width-1, _height-1);

    _rs = 1;
    _cs = 0;

    for(y=_height; y>0; y--) {
        for(x=_width; x>0; x--) {
            lcdPort.write( hi );
            lcdPort.write( lo );
        }
    }

    _cs = 1;
}


void Adafruit_ST7735::pushColor(uint16_t color)
{
    _rs = 1;
    _cs = 0;

    lcdPort.write( color >> 8 );
    lcdPort.write( color );
    _cs = 1;
}


void Adafruit_ST7735::drawPixel(int16_t x, int16_t y, uint16_t color)
{

    if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

    setAddrWindow(x,y,x+1,y+1);

    _rs = 1;
    _cs = 0;
    
    lcdPort.write( color >> 8 );
    lcdPort.write( color );

    _cs = 1;
}


void Adafruit_ST7735::drawFastVLine(int16_t x, int16_t y, int16_t h,
                                    uint16_t color)
{

    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;
    if((y+h-1) >= _height) h = _height-y;
    setAddrWindow(x, y, x, y+h-1);

    uint8_t hi = color >> 8, lo = color;
    _rs = 1;
    _cs = 0;
    while (h--) {
        lcdPort.write( hi );
        lcdPort.write( lo );
    }
    _cs = 1;
}


void Adafruit_ST7735::drawFastHLine(int16_t x, int16_t y, int16_t w,
                                    uint16_t color)
{

    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;
    if((x+w-1) >= _width)  w = _width-x;
    setAddrWindow(x, y, x+w-1, y);

    uint8_t hi = color >> 8, lo = color;
    _rs = 1;
    _cs = 0;
    while (w--) {
        lcdPort.write( hi );
        lcdPort.write( lo );
    }
    _cs = 1;
}


// fill a rectangle
void Adafruit_ST7735::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                               uint16_t color)
{

    // rudimentary clipping (drawChar w/big text requires this)
    if((x >= _width) || (y >= _height)) return;
    if((x + w - 1) >= _width)  w = _width  - x;
    if((y + h - 1) >= _height) h = _height - y;

    setAddrWindow(x, y, x+w-1, y+h-1);

    uint8_t hi = color >> 8, lo = color;
    _rs = 1;
    _cs = 0;
    for(y=h; y>0; y--) {
        for(x=w; x>0; x--) {
            lcdPort.write( hi );
            lcdPort.write( lo );
        }
    }

    _cs = 1;
}


// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t Adafruit_ST7735::Color565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_MH  0x04

void Adafruit_ST7735::setRotation(uint8_t m)
{

    writecommand(ST7735_MADCTL);
    rotation = m % 4; // can't be higher than 3
    switch (rotation) {
        case 0:
            writedata(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
            _width  = ST7735_TFTWIDTH;
            _height = ST7735_TFTHEIGHT;
            break;
        case 1:
            writedata(MADCTL_MY | MADCTL_MV | MADCTL_RGB);
            _width  = ST7735_TFTHEIGHT;
            _height = ST7735_TFTWIDTH;
            break;
        case 2:
            writedata(MADCTL_RGB);
            _width  = ST7735_TFTWIDTH;
            _height = ST7735_TFTHEIGHT;
            break;
        case 3:
            writedata(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
            _width  = ST7735_TFTHEIGHT;
            _height = ST7735_TFTWIDTH;
            break;
    }
}


void Adafruit_ST7735::invertDisplay(boolean i)
{
    writecommand(i ? ST7735_INVON : ST7735_INVOFF);
}


int Adafruit_ST7735::BMP_16(unsigned int x, unsigned int y, const char *Name_BMP)
{
 
 
 
#define OffsetPixelWidth    18
#define OffsetPixelHeigh    22
#define OffsetFileSize      34
#define OffsetPixData       10
#define OffsetBPP           28
    pc.printf("TEST START\r\n");
    char filename[50];
    unsigned char BMP_Header[54];
    unsigned short BPP_t;
    unsigned int PixelWidth,PixelHeigh,start_data;
    unsigned int    i,off;
    int padd,j;
    unsigned short *line;
 
    pc.printf("TEST START1\r\n");
    // get the filename
    i=0;
    while (*Name_BMP!='\0') {
        filename[i++]=*Name_BMP++;
    }
    pc.printf("TEST START2\r\n");
    filename[i] = 0;  
    
    pc.printf("TEST START3\r\n");
    FILE *Image = fopen((const char *)&filename[0], "rb");  // open the bmp file
    pc.printf("TEST FILEOPEN\r\n");
    if (!Image) {
        pc.printf("TEST error file not found");
        return(0);      // error file not found !
    }
 
    fread(&BMP_Header[0],1,54,Image);      // get the BMP Header
 
    if (BMP_Header[0] != 0x42 || BMP_Header[1] != 0x4D) {  // check magic byte
        pc.printf("TEST error no BMP file");
        fclose(Image);
        return(-1);     // error no BMP file
    }
 
    BPP_t = BMP_Header[OffsetBPP] + (BMP_Header[OffsetBPP + 1] << 8);
    if (BPP_t != 0x0010) {
        pc.printf("TEST error no 16 bit BMP\r\n");
        fclose(Image);
        return(-2);     // error no 16 bit BMP
    }
 
    PixelHeigh = BMP_Header[OffsetPixelHeigh] + (BMP_Header[OffsetPixelHeigh + 1] << 8) + (BMP_Header[OffsetPixelHeigh + 2] << 16) + (BMP_Header[OffsetPixelHeigh + 3] << 24);
    PixelWidth = BMP_Header[OffsetPixelWidth] + (BMP_Header[OffsetPixelWidth + 1] << 8) + (BMP_Header[OffsetPixelWidth + 2] << 16) + (BMP_Header[OffsetPixelWidth + 3] << 24);
    if (PixelHeigh > _height + y || PixelWidth > _width + x) {
        pc.printf("TEST to big\r\n");
        fclose(Image);
        return(-3);      // to big
    }
 
    start_data = BMP_Header[OffsetPixData] + (BMP_Header[OffsetPixData + 1] << 8) + (BMP_Header[OffsetPixData + 2] << 16) + (BMP_Header[OffsetPixData + 3] << 24);
 
    line = (unsigned short *) malloc (2 * PixelHeigh); // we need a buffer for a line
    if (line == NULL) {
        pc.printf("TEST error no memory\r\n");
        return(-4);         // error no memory
    }
    pc.printf("TEST 506");
    // the bmp lines are padded to multiple of 4 bytes
    padd = -1;
    do {pc.printf("TEST padd : %d\r\n",padd);
        padd ++;
    } while ((PixelHeigh * 2 + padd)%4 != 0);
 
    setAddrWindow(x, y,PixelWidth ,PixelHeigh);
    writecommand(0x2C);  // send pixel
    
    pc.printf("TEST 518");
    for (j = PixelWidth - 1; j >= 0; j--) {               //Lines bottom up
        off = j * (PixelHeigh  * 2 + padd) + start_data;   // start of line
        fseek(Image, off ,SEEK_SET);
        fread(line,1,PixelHeigh * 2,Image);       // read a line - slow 
        for (i = 0; i < PixelHeigh; i++) {        // copy pixel data to TFT
            _rs = 1;
            _cs = 0;
            lcdPort.write(line[i]);
        }
     }
    _cs = 1;
    lcdPort.format(8,3);
    free (line);
    fclose(Image);
    setAddrWindow(0,0,_width,_height);
    pc.printf("TEST END");
    return(1);
}

#define RGB(r,g,b)  (((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3)) //5 red | 6 green | 5 blue
#define TFT_DEBUG

int Adafruit_ST7735::DrawBitmapFile(const char *Name_BMP)
{
   
    char img[3*240];
    uint32_t imgsize = 0;
    uint32_t offset = 0;
    uint32_t imgw = 0;
    uint32_t imgh = 0;
    char colbits = 0;
    char compress = 0;
    uint16_t col;

    int i, j;
    
    char filename[50];
    
    pc.printf("TEST START1\r\n");
    // get the filename
    i=0;
    while (*Name_BMP!='\0') {
        filename[i++]=*Name_BMP++;
    }
    pc.printf("TEST START2\r\n");
    filename[i] = 0;  
    
    pc.printf("TEST START3\r\n");
    FILE *Image = fopen((const char *)&filename[0], "rb");  // open the bmp file
    pc.printf("TEST FILEOPEN\r\n");

    if(Image == NULL) return -1;
    if(fgetc(Image) != 0x42) return -2;
    if(fgetc(Image) != 0x4D) return -2;

    for(i = 0; i < 4; i++)
    {
        imgsize += (((uint32_t)fgetc(Image)) << i*8);
    }
#ifdef TFT_DEBUG
    pc.printf("BMP SIZE:%d\r\n",imgsize);
#endif
    fseek(Image,4,SEEK_CUR);
    for(i = 0; i < 4; i++)
    {
        offset += (((uint32_t)fgetc(Image)) << i*8);
    }
#ifdef TFT_DEBUG    
    pc.printf("BMP OFFSET:%d\r\n",offset);
#endif
    fseek(Image,4,SEEK_CUR);
    for(i = 0; i < 4; i++)
    {
        imgw += (((uint32_t)fgetc(Image)) << i*8);
    }
    if(imgw > 240) return -3;
    
    for(i = 0; i < 4; i++)
    {
        imgh += (((uint32_t)fgetc(Image)) << i*8);
    }
    if(imgh > 320) return -3;
    
    fseek(Image,2,SEEK_CUR);
    colbits = fgetc(Image);
    //if(colbits != 16 || colbits != 24) return -4;
    fgetc(Image);
    if((compress=fgetc(Image)) != 0)
    {
    #ifdef TFT_DEBUG    
        pc.printf("Not supported compress : %d\r\n",compress);
    #endif
        return -4;    
    }
    

#ifdef TFT_DEBUG    
    pc.printf("RESOL : %d col, %d X %d",colbits,imgw,imgh);
#endif    
    
    fseek(Image, offset, SEEK_SET);
    for (j = imgh; j >= 0; j--)        //Lines
    {  
        fread(img,sizeof(char),imgw*3,Image);
        _cs = 1;
        setAddrWindow(0, j, imgw ,1);
        writecommand(0x2C);  // send pixel
        #ifdef TARGET_WIZWIKI_W7500
        lcdPort.format(16,3);
        #endif

        for(i = 0; i < imgw; i++)
        {
/*            if(colbits == 16)
            {
                col = (uint16_t)img[2*i+1];
                col <<= 8;
                col += (uint16_t)img[2*i];
            }
            else if(colbits == 24) */
            //{
                col = RGB((uint16_t)img[3*i+2],(uint16_t)img[3*i+1], (uint16_t)img[3*i]);
            //}
        #ifdef TFT_DEBUG    
        /*
        
           pc.printf("RGB(%d): ",i);
           pc.printf("(%d,",img[3*i+2]);
           pc.printf("%d,", img[3*i+1]);
           pc.printf("%d),->", img[3*i]);
           pc.printf("%04x\r\n",col);
        */
           //pc.printf("RGB(%d): (%d,%d,%d) -> %04X\r\n ",i,img[3*i+2],img[3*i+1],img[3*i],col);
        #endif    
            _rs = 1;
            _cs = 0;

            lcdPort.write(col);
        }
        _cs = 1;
        #ifdef TARGET_WIZWIKI_W7500
        lcdPort.format(8,3);
        #endif
    }
    setAddrWindow(0,0,_width,_height);
    
    return 0;
}


