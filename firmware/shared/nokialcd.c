/*--------------------------------------------------------------------------*
* Nokia LCD Interface
*---------------------------------------------------------------------------*
* 26-Apr-2014 ShaneG
*
* Simple routines for controlling a Nokia LCD.
*--------------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "../hardware.h"
#include "utility.h"
#include "iohelp.h"
#include "smallfont.h"
#include "nokialcd.h"

// Only provide the functions if the driver is enabled
#ifdef LCD_ENABLED

/** Send a data byte to the LCD
 *
 * @param data the data byte to send.
 */
void lcdData(uint8_t data) {
  // Bring CD high
  PORTB |= (1 << LCD_CD);
  // Send the data
  sspiOutMSB(LCD_SCK, LCD_MOSI, data, 8);
  }

/** Send a command byte to the LCD
 *
 * @param cmd the command byte to send.
 */
void lcdCommand(uint8_t cmd) {
  // Bring CD low
  PORTB &= ~(1 << LCD_CD);
  // Send the data
  sspiOutMSB(LCD_SCK, LCD_MOSI, cmd, 8);
  }

/** Initialise the LCD
 *
 * Sets up the pins required to communicate with the LCD screen and then does
 * the actual chipset initialisation. The pin numbers to use are defined in
 * @ref hardware.h.
 */
void lcdInit() {
  // Set up the output pins, ensuring they are all 'low' to start
  uint8_t val = (1 << LCD_SCK) | (1 << LCD_MOSI) | (1 << LCD_RESET) | (1 << LCD_CD);
  PORTB &= ~val;
  DDRB |= val;
  // Do a hard reset on the LCD
  wait(10);
  PORTB |= (1 << LCD_RESET);
  // Initialise the LCD
  lcdCommand(0x21);  // LCD Extended Commands.
  lcdCommand(0xA1);  // Set LCD Vop (Contrast).
  lcdCommand(0x04);  // Set Temp coefficent.
  lcdCommand(0x14);  // LCD bias mode 1:48.
  lcdCommand(0x20);  // LCD Normal commands
  lcdCommand(0x0C);  // Normal display, horizontal addressing
  }

/** Clear the screen
 *
 * Clear the entire display.
 *
 * @param invert if true the colors are inverted and the screen will be filled
 *               with black.
 */
void lcdClear(bool invert) {
  uint8_t fill = invert?0xFF:0x00;
  // Set the position
  lcdCommand(0x80);
  lcdCommand(0x40);
  // Fill in the whole display
  for(uint16_t index = 0; index < (LCD_COL * LCD_ROW); index++)
    lcdData(fill);
  }

/** Clear a single row
 *
 * Clears a single character row from the left edge of the screen to the right.
 *
 * @param row the row number (0 to 5) to clear.
 * @param invert if true the colors are inverted and the screen will be filled
 *               with black.
 */
void lcdClearRow(uint8_t row, bool invert) {
  uint8_t fill = invert?0xFF:0x00;
  // Set the position
  lcdCommand(0x80);
  lcdCommand(0x40 | (row % LCD_ROW));
  // Fill in the row
  for(uint8_t index = 0; index < LCD_COL; index++)
    lcdData(fill);
  }

/** Write a single character
 *
 * Display a single ASCII character at the position described by the row and
 * column parameters. Note that the row indicates an 8 pixel high character
 * row while the column represents individual pixels. This code uses a built
 * in character set where each character is 5 pixels wide and adds a single
 * column of pixels as spacing giving a total width of 6 pixels.
 *
 * @param row the row number (0 to 5) to display the character.
 * @param col the column position (0 to 83) for the start of the left side of
 *            the character.
 * @param ch  the character to display. If the character is out of range it
 *            will be replaced with the '?' character.
 * @param invert if true the colors are inverted and the character will be
 *               displayed as white on black.
 */
void lcdPrintChar(uint8_t row, uint8_t col, char ch, bool invert) {
  // Make sure it is on the screen
  if((row>=LCD_ROW)||(col>=LCD_COL))
    return;
  // If the character is invalid replace it with the '?'
  if((ch<0x20)||(ch>0x7f))
    ch = '?';
  // Set the starting address
  lcdCommand(0x80 | col);
  lcdCommand(0x40 | (row % LCD_ROW));
  // And send the column data
  const uint8_t *chdata = SMALL_FONT + ((ch - 0x20) * 5);
  for(uint8_t pixels = 0; (pixels < DATA_WIDTH) && (col < LCD_COL); pixels++, col++, chdata++) {
    uint8_t data = pgm_read_byte_near(chdata);
    lcdData(invert?~data:data);
    }
  // Add the padding byte
  if(col < LCD_COL)
    lcdData(invert?0xFF:0x00);
  }

/** Write a nul terminated string
 *
 * Display a string of ASCII characters at the position described by the row
 * and column parameters. Note that the row indicates an 8 pixel high character
 * row while the column represents individual pixels. This code uses a built
 * in character set where each character is 6 pixels wide.
 *
 * This function does not perform any word wrapping or other text layout
 * functions. If the string exceeds the size of the display it will simply
 * be clipped.
 *
 * @param row the row number (0 to 5) to display the character.
 * @param col the column position (0 to 83) for the start of the left side of
 *            the character.
 * @param str the string to display. If a character in the string is out of
 *            range it will be replaced with the '?' character.
 * @param invert if true the colors are inverted and the character will be
 *               displayed as white on black.
 */
void lcdPrint(uint8_t row, uint8_t col, const char *str, bool invert) {
  for(;(*str!='\0')&&(col<LCD_COL);col+=CHAR_WIDTH,str++)
    lcdPrintChar(row, col, *str, invert);
  }

/** Write a nul terminated string from PROGMEM
 *
 * Display a string of ASCII characters at the position described by the row
 * and column parameters. Note that the row indicates an 8 pixel high character
 * row while the column represents individual pixels. This code uses a built
 * in character set where each character is 6 pixels wide.
 *
 * This function does not perform any word wrapping or other text layout
 * functions. If the string exceeds the size of the display it will simply
 * be clipped.
 *
 * @param row the row number (0 to 5) to display the character.
 * @param col the column position (0 to 83) for the start of the left side of
 *            the character.
 * @param str the string to display. If a character in the string is out of
 *            range it will be replaced with the '?' character.
 * @param invert if true the colors are inverted and the character will be
 *               displayed as white on black.
 */
void lcdPrintP(uint8_t row, uint8_t col, const char *str, bool invert) {
  while(col<LCD_COL) {
    char ch = pgm_read_byte_near(str);
    if(ch=='\0')
      return;
    lcdPrintChar(row, col, ch, invert);
    col += CHAR_WIDTH;
    str++;
    }
  }

/** Display an arbitrary graphic on the display
 *
 * This function is used to display user defined graphics held in PROGMEM to
 * the display. The first byte of the graphic indicates the height (in 8 pixel
 * rows) and the width (in pixel columns) packed as HHWWWWWW giving a maximum
 * size of 4 rows (32 pixels) by 64 pixels for a single image. Images that
 * will display off the edge of the screen are clipped. The packed version of
 * the width and height are 1 less than the actual width and height.
 *
 * The image is packed as a sequence of 8 pixel vertical strips.
 *
 * @param row the row number (0 to 5) to start the top left of the image.
 * @param col the column number (0 to 83) to start the top left of the image.
 * @param img pointer to the image in PROGMEM.
 * @param invert if true the pixel colors will be inverted.
 */
void lcdImageP(uint8_t row, uint8_t col, const uint8_t *img, bool invert) {
  // Break out the width and height
  uint8_t data = pgm_read_byte_near(img++);
  uint8_t height = (data >> 6) + 1;
  uint8_t width = (data & 0x03F) + 1;
  while((height>0)&&(row<LCD_ROW)) {
    // Set the starting address
    lcdCommand(0x80 | col);
    lcdCommand(0x40 | row);
    // Send the data (up to the end of the screen)
    for(uint8_t offset=0; offset<width; offset++, img++) {
      if((col+offset)<LCD_COL) {
        if(invert)
          lcdData(~pgm_read_byte_near(img));
        else
          lcdData(pgm_read_byte_near(img));
        }
      }
    height--;
    row++;
    }
  }

// Only provide the functions if the driver is enabled
#endif /* LCD_ENABLED */

