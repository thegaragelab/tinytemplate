/*--------------------------------------------------------------------------*
* General utility functions
*---------------------------------------------------------------------------*
* 15-Apr-2014 ShaneG
*
* A collection of utility functions that don't really fit anywhere else.
*--------------------------------------------------------------------------*/
#ifndef __UTILITY_H
#define __UTILITY_H

//--- Required definitions
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Convert the low four bits of a byte value into an upper case hex digit.
 *
 * @param value the value to convert
 *
 * @return the ASCII code for the appropriate hex digit.
 */
char hexChar(uint8_t value);

#ifdef __cplusplus
  }
#endif

#endif /* __UTILITY_H */
