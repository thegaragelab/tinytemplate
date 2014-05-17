/*--------------------------------------------------------------------------*
* PWM output helpers
*---------------------------------------------------------------------------*
* 14-May-2014 ShaneG
*
* These functions provide simulated analog output using PWM channels on
* TIMER0
*--------------------------------------------------------------------------*/
#include <avr/io.h>
#include "iohelp.h"

/** Initialise the PWM system
 *
 * Sets up PWM support. The library only supports hardware PWM on TIMER0
 * so you only get up to two PWM pins - PB0 and PB1.
 */
void pwmInit() {
  TCCR0A = (3<<WGM00); // Enable fast PWM mode
  TCCR0B = (1<<CS00);  // No prescaler
  }

/** Set the value of a PWM pin
 *
 * Set the output for a PWM pin.
 *
 * @param pwm   the PWM output to change
 * @param value the value for the duty cycle ranging from 0 (fully off) to 255
 *              (fully on).
 */
void pwmOut(PWM pwm, uint8_t value) {
  // Make sure the pin is an output
  DDRB |= (1 << pwm);
  if(pwm==PWM0) {
    TCCR0A |= (2<<COM0A0);
    OCR0A = value;
    }
  else if(pwm==PWM1) {
    TCCR0A |= (2<<COM0B0);
    OCR0B = value;
    }
  }

