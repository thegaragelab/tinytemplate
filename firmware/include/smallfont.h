/*--------------------------------------------------------------------------*
* Small font
*---------------------------------------------------------------------------*
* 26-Apr-2014 ShaneG
*
* A simple font for use with LCD/OLED displays.
*--------------------------------------------------------------------------*/
#ifndef __SMALLFONT_H
#define __SMALLFONT_H

#include <stdint.h>
#include <avr/pgmspace.h>


/** Glyph data (stored in PROGMEM)
 */
extern const uint8_t SMALL_FONT[] PROGMEM;

/** Width of the font glyphs (in bytes)
 */
#define DATA_WIDTH 5

/** Width of the displayed character
 *
 * An extra byte is added to the font data for character spacing.
 */
#define CHAR_WIDTH (DATA_WIDTH + 1)

#endif /* __SMALLFONT_H */
