// YJD1602A/JLX1602A LCD Driver Library in I2C mode
#include "LCD_I2C.h"

LCD_I2C::LCD_I2C(void) {
  init(0);
}

void LCD_I2C::init(uint8_t fourbitmode) {
  if (fourbitmode)
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  else 
    _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
}

void LCD_I2C::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  if (lines > 1) {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;
  _currline = 0;

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != 0) && (lines == 1)) {
    _displayfunction |= LCD_5x10DOTS;
  }

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands.
  delayMicroseconds(50000); 
  
  //put the LCD into 4 bit or 8 bit mode
  if (!(_displayfunction & LCD_8BITMODE)) {
    // this is according to the hitachi HD44780 datasheet, figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(0x03);
    delayMicroseconds(4500); // wait min 4.1ms

    // second try
    write4bits(0x03);
    delayMicroseconds(4500); // wait min 4.1ms
    
    // third go!
    write4bits(0x03); 
    delayMicroseconds(150);

    // finally, set to 4-bit interface
    write4bits(0x02); 
  } else {
    // this is according to the hitachi HD44780 datasheet, page 45 figure 23

    // Send function set command sequence
    command(LCD_FUNCTIONSET | _displayfunction);
    delayMicroseconds(4500);  // wait more than 4.1ms

    // second try
    command(LCD_FUNCTIONSET | _displayfunction);
    delayMicroseconds(150);

    // third go
    command(LCD_FUNCTIONSET | _displayfunction);
  }

  // finally, set # lines, font size, etc.
  command(LCD_FUNCTIONSET | _displayfunction);  

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  display();

  // clear it off
  clear();

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);
}

//// high level commands, for the user

void LCD_I2C::clear() {
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void LCD_I2C::home() {
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void LCD_I2C::setCursor(uint8_t col, uint8_t row) {
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row >= _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }
  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LCD_I2C::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCD_I2C::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LCD_I2C::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCD_I2C::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LCD_I2C::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCD_I2C::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LCD_I2C::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LCD_I2C::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LCD_I2C::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LCD_I2C::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LCD_I2C::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LCD_I2C::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations with custom characters
void LCD_I2C::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  Wire.beginTransmission((byte)LCD_SLAVE_ADDR);
  Wire.write((byte)0x80);
  Wire.write((byte)(LCD_SETCGRAMADDR | (location << 3)));
  Wire.write((byte)0x40); //Co=0,A0=1,data
  for (int i=0; i<8; i++) {
    Wire.write((byte)charmap[i]);
  }
  Wire.endTransmission();
}

//// mid level commands, for sending data/commands

inline void LCD_I2C::command(uint8_t value) {
  Wire.beginTransmission((byte)LCD_SLAVE_ADDR);
  Wire.write((byte)0x80); //Co=1,A0=0,command
  Wire.write((byte)value);
  Wire.endTransmission();
}

inline size_t LCD_I2C::write(uint8_t value) {
  Wire.beginTransmission((byte)LCD_SLAVE_ADDR);
  Wire.write((byte)0x40); //Co=0,A0=1,data
  Wire.write((byte)value);
  Wire.endTransmission();
  //return 1; // assume sucess
}

//// low level data pushing commands

void LCD_I2C::pulseEnable(void) {
}

void LCD_I2C::write4bits(uint8_t value) {
}

void LCD_I2C::write8bits(uint8_t value) {
}
