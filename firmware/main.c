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
#include "utility.h"

// Forward declaration with 'noreturn' attribute
void main() __attribute__ ((noreturn));

/** Program entry point
 */
void main() {
  uint16_t count = 0;
  uartInit();
  while(true) {
    count++;
    PRINTF("1: %c\n", count);
    PRINTF("2: %s, %c\n", "string", count);
    PRINTF("3: %c, %s, %c\n", count, "string", count);
    PRINTF("4: %c, %c, %s\n", count, count, "string");
    PRINTF("5: %c, %c, %s, %c\n", count, count, "string", count);
    wait(1000);
    }
  }
