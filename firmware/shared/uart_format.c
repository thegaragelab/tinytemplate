/*--------------------------------------------------------------------------*
* Formatted serial output functions
*---------------------------------------------------------------------------*
* 14-Apr-2014 ShaneG
*
* Provides some simple formatted output functions for serial communications.
*--------------------------------------------------------------------------*/
#include <stdarg.h>
#include <stdbool.h>
#include "../hardware.h"
#include "softuart.h"

// Only if enabled
#ifdef UART_ENABLED

//---------------------------------------------------------------------------
// Helper functions
//---------------------------------------------------------------------------

/** Do the actual formatting
 *
 * This function uses the current two characters of the input string to
 * determine what to send to the serial port.
 *
 * @param ch1 the current character of the format string
 * @param ch2 the next character of the format string
 * @param args the argument list containing the embedded items
 *
 * @return true if both characters should be skipped, false if we only need
 *              to move ahead by one.
 */
static bool printFormat(char ch1, char ch2, va_list *args) {
  bool skip = true;
  // Fail fast
  if(ch1=='%') {
    // Use the second character to determine what is requested
    if((ch2=='%')||(ch2=='\0'))
      uartSend('%');
    else if(ch2=='c')
      uartSend(va_arg(*args, int));
    else if(ch2=='u')
      uartInt(va_arg(*args, unsigned int));
    else if(ch2=='x')
      uartHex(va_arg(*args, unsigned int));
    else if(ch2=='s')
      uartPrint(va_arg(*args, char *));
    else if(ch2=='S')
      uartPrintP(va_arg(*args, char *));
    }
  else {
    uartSend(ch1);
    skip = false;
    }
  return skip;
  }

//---------------------------------------------------------------------------
// Public API
//---------------------------------------------------------------------------

/** Print a formatted string from RAM
 *
 * Prints a formatted string from RAM to the serial port. This function
 * supports a subset of the 'printf' string formatting syntax. Allowable
 * insertion types are:
 *
 *  %% - Display a % character. There should be no entry in the variable
 *       argument list for this entry.
 *  %u - Display an unsigned integer in decimal. The matching argument may
 *       be any 16 bit value.
 *  %x - Display an unsigned integer in hexadecimal. The matching argument may
 *       be any 16 bit value.
 *  %c - Display a single ASCII character. The matching argument may be any 8
 *       bit value.
 *  %s - Display a NUL terminated string from RAM. The matching argument must
 *       be a pointer to a RAM location.
 *  %S - Display a NUL terminated string from PROGMEM. The matching argument
 *       must be a pointer to a PROGMEM location.
 *
 * @param cszString pointer to a nul terminated format string in RAM.
 */
void uartFormat(const char *cszString, ...) {
  va_list args;
  va_start(args, cszString);
  char ch1, ch2 = *cszString;
  for(int index=1; ch2!='\0'; index++) {
    ch1 = ch2;
    ch2 = cszString[index];
    if(printFormat(ch1, ch2, &args)) {
      // Move ahead an extra character so we wind up jumping by two
      ch1 = ch2;
      ch2 = cszString[++index];
      }
    }
  va_end(args);
  }

/** Print a formatted string from PROGMEM
 *
 * Prints a formatted string from PROGMEM to the serial port. This function
 * supports a subset of the 'printf' string formatting syntax. Allowable
 * insertion types are:
 *
 *  %% - Display a % character. There should be no entry in the variable
 *       argument list for this entry.
 *  %u - Display an unsigned integer in decimal. The matching argument may
 *       be any 16 bit value.
 *  %x - Display an unsigned integer in hexadecimal. The matching argument may
 *       be any 16 bit value.
 *  %c - Display a single ASCII character. The matching argument may be any 8
 *       bit value.
 *  %s - Display a NUL terminated string from RAM. The matching argument must
 *       be a pointer to a RAM location.
 *  %S - Display a NUL terminated string from PROGMEM. The matching argument
 *       must be a pointer to a PROGMEM location.
 *
 * @param cszString pointer to a nul terminated format string in PROGMEM.
 */
void uartFormatP(const char *cszString, ...) {
  va_list args;
  va_start(args, cszString);
  char ch2 = pgm_read_byte_near(cszString), ch1 = ch2;
  for(int index=1; ch2!='\0'; index++) {
    ch1 = ch2;
    ch2 = pgm_read_byte_near(cszString + index);
    if(printFormat(ch1, ch2, &args)) {
      // Move ahead an extra character so we wind up jumping by two
      index++;
      ch1 = ch2;
      ch2 = pgm_read_byte_near(cszString + index);
      }
    }
  va_end(args);
  }

#endif /* UART_ENABLED */

