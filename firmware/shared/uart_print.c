/*--------------------------------------------------------------------------*
* Formatted serial output functions
*---------------------------------------------------------------------------*
* 14-Apr-2014 ShaneG
*
* Provides some simple formatted output functions for serial communications.
*--------------------------------------------------------------------------*/
#include "softuart.h"

/** Print a string from RAM
 *
 * This function simply prints the nul terminated string from RAM to the
 * serial port.
 *
 * @param cszString pointer to a character array in RAM.
 */
void uartPrint(const char *cszString) {
  while(*cszString) {
    uartSend(*cszString);
    cszString++;
    }
  }

/** Print a string from PROGMEM
 *
 * This function simply prints the nul terminated string from PROGMEM to the
 * serial port.
 *
 * @param cszString pointer to a character array in PROGMEM.
 */
void uartPrintP(const char *cszString) {
  }

/** Print an unsigned 16 bit value in decimal
 *
 * Print the given value in decimal format to the UART.
 *
 * @param value the value to print.
 */
void uartInt(uint16_t value) {
  }

/** Print an unsigned 16 bit value in hexadecimal
 *
 * Print the given value in hexadecimal format to the UART.
 *
 * @param value the value to print.
 */
void uartHex(uint16_t value) {
  }
