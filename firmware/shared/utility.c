/*--------------------------------------------------------------------------*
* General utility functions
*---------------------------------------------------------------------------*
* 15-Apr-2014 ShaneG
*
* A collection of utility functions that don't really fit anywhere else.
*--------------------------------------------------------------------------*/
#include "utility.h"

// Determine the inner loop for the 'wait()' function. This is based on 6
// instruction cycles per loop.
#define WAIT_INNER (F_CPU / 6000)

/** An inaccurate delay function
 *
 * This function will delay for the given number of milliseconds. This is not
 * an accurate delay (interrupt activity will interfere with the timing and it
 * uses instruction timing approximations to consume the time) but should be
 * close enough for many purposes without having to revert to a timer interrupt.
 */
void wait(uint16_t millis) {
  for(uint16_t outer=0; outer<millis; outer++) {
    for(uint16_t inner=0; inner<WAIT_INNER; inner++) {
      asm volatile(
        "  nop    \n\t"
        );
      }
    }
  }

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

