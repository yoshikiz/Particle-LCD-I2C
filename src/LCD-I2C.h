#ifndef LCD_I2C_h
#define LCD_I2C_h

#include "application.h"
#include <Wire.h>
#include <Print.h>

#define LCD_SLAVE_ADDR 0x3C

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class LCD-I2C : public Print {
public:
  LCD-I2C (void);
  void begin (uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
  void clear ();
  void home ();

  void noDisplay ();
  void display ();
  void noBlink ();
  void blink ();
  void noCursor ();
  void cursor ();
  void scrollDisplayLeft ();
  void scrollDisplayRight ();
  void leftToRight ();
  void rightToLeft ();
  void autoscroll ();
  void noAutoscroll ();

  void createChar (uint8_t, uint8_t[]);
  void setCursor (uint8_t, uint8_t); 
  void command (uint8_t);
  virtual size_t write (uint8_t);
  using Print::write;

private:
  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;
  uint8_t _cols, _rows, _charsize;
};

#endif
