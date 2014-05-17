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
#include <avr/interrupt.h>
#include "softuart.h"
#include "iohelp.h"
#include "utility.h"

// Forward declaration with 'noreturn' attribute
void main() __attribute__ ((noreturn));

/** Program entry point
 */
void main() {
  spwmInit();
  sei();
  spwmOut(SPWM0, 64);  // 25% duty cycle
  spwmOut(SPWM1, 172); // 75% duty cycle
  spwmOut(SPWM2, 255); // 100% duty cycle
  while(true);
  }
