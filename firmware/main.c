/*--------------------------------------------------------------------------*
* Main program
*---------------------------------------------------------------------------*
* 14-Apr-2014 ShaneG
*
* Template program for ATtiny85 C/asm projects.
*--------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include "softuart.h"

/** Program entry point
 */
int main() {
  uint16_t count = 0;
  PRINT("Starting program.");
  while(true) {
    PRINTF("count = %u", count);
    // Do nothing
    }
  return 0;
  }
