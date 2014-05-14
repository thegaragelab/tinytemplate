/*--------------------------------------------------------------------------*
* System ticks implementation
*---------------------------------------------------------------------------*
* 14-May-2014 ShaneG
*
* Provide a system tick reference to measuring longer periods of time.
*--------------------------------------------------------------------------*/
#include <avr/io.h>
#include "systicks.h"

// Only include if enabled
#ifdef SYSTICK_ENABLED

//! The main system tick counter
static volatile uint16_t g_systicks = 0;

//! The 'ticklet' counter - used to count 1/256 of a tick
static volatile uint8_t  g_ticklet = 0;

/** Initialise the 'ticks' subsystem.
 *
 * This function sets up the required interrupts and initialises the tick
 * counter. A tick occurs approximately every 0.01 seconds (100Hz) and they
 * are tracked by a 16 bit counter. A program can use the difference between
 * samples of the counter to measure longer periods of time (up to about 11
 * minutes).
 *
 * The ticks system uses TIMER1 to provide the interrupt used to update the
 * time count (this interrupt is shared with the software PWM implementation
 * as well).
 */
void ticksInit() {
  TCCR1 = (1 << CS10) | (1 << CS13);
  GTCCR &= 0x81; // Clear all TIMER1 bits
  TIMSK |= (1 << TOIE1);
  }

/** Get the current tick count
 *
 * @return the current tick count. The count wraps around once it reaches
 *         65535 (about 10.9 minutes).
 */
uint16_t ticks() {
  return g_systicks;
  }

/** Calculate the ticks elapsed since the given sample.
 *
 * This function calculates the number of ticks that have elapsed since the
 * reference sample was taken. The function takes into account the wrap around
 * of the counter (but it cannot detect multiple wrap arounds).
 *
 * @param reference the reference tick count to calculate against.
 *
 * @return the number of ticks elapsed since the reference count.
 */
uint16_t ticksElapsed(uint16_t reference) {
  uint16_t now = g_systicks;
  if(now<reference)
    return now + (65535 - reference);
  return now - reference;
  }

/** Interrupt handler
 */
ISR(TIMER1_OVF_vect) {
  // Update the 'ticklet' count
  g_ticklet++;
  if(g_ticklet!=0)
    return;
  // If 'ticklet' wraps around, update the tick count
  g_systicks++;
  }

#endif /* SYSTICK_ENABLED */

