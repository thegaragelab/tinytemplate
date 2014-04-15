/*--------------------------------------------------------------------------*
* General utility functions
*---------------------------------------------------------------------------*
* 15-Apr-2014 ShaneG
*
* A collection of utility functions that don't really fit anywhere else.
*--------------------------------------------------------------------------*/
#include "utility.h"

/** Convert the low four bits of a byte value into an upper case hex digit.
 *
 * @param value the value to convert
 *
 * @return the ASCII code for the appropriate hex digit.
 */
char hexChar(uint8_t value) {
  value &= 0x0F;
  if(value<10)
    return '0' + value;
  return value + 'A' - 10;
  }

