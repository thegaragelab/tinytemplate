/*--------------------------------------------------------------------------*
* System ticks implementation
*---------------------------------------------------------------------------*
* 14-May-2014 ShaneG
*
* Provide a system tick reference to measuring longer periods of time.
*--------------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../hardware.h"
#include "systicks.h"
#include "iohelp.h"

// Maximum number of software PWM outputs
#define SPWM_MAX 4

// Number of 'ticklets' per tick
#define TICKLETS 64

// If software PWM is enabled, we get systick as well
#ifdef SOFTPWM_ENABLED
#  define SYSTICK_ENABLED
   // Make sure we don't have too many outputs
#  if SPWM_COUNT > SPWM_MAX
#    error "You have defined too many software PWM outputs"
#  endif
#endif

//---------------------------------------------------------------------------
// System ticks implementation
//---------------------------------------------------------------------------

// Only include if enabled
#ifdef SYSTICK_ENABLED

//! The main system tick counter
static volatile uint16_t g_systicks = 0;

//! The 'ticklet' counter - used to count a portion of a tick
static volatile uint8_t  g_ticklet = 0;

/** Initialise the 'ticks' subsystem.
 *
 * This function sets up the required interrupts and initialises the tick
 * counter. There are approximately TICKS_PER_SECOND ticks every second that
 * elapses (depending on interrupt activity and clock accuracy) and they are
 * tracked by a 16 bit counter. A program can use the difference between
 * samples of the counter to measure longer periods of time.
 *
 * The ticks system uses TIMER1 to provide the interrupt used to update the
 * time count (this interrupt is shared with the software PWM implementation
 * as well).
 */
void ticksInit() {
  // Stop timer and clear prescaler
  TCCR1 = 0;
  TCNT1 = 0;
  GTCCR &= 0x81;
  GTCCR |= (1 << PSR1);
  // Set up the prescaler and enable overflow interrupt
  TCCR1 = (1 << CS12); // Divide by 8
  TIMSK |= (1 << TOIE1);
  }

/** Get the current tick count
 *
 * @return the current tick count. The count wraps around once it reaches
 *         65535.
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

#endif /* SYSTICK_ENABLED */

//---------------------------------------------------------------------------
// Software PWM implementation
//---------------------------------------------------------------------------

#ifdef SOFTPWM_ENABLED

//! The current PWM values
static uint8_t g_pwmout[SPWM_COUNT];

/** Initialise the software PWM system
 *
 * Sets up software PWM support. Using the software PWM implementation also
 * enables the system ticks module (they share the same interrupt).
 */
void spwmInit() {
  // Set everything to zero for a start
  for(uint8_t index=0; index<SPWM_COUNT; index++)
    g_pwmout[index] = 0;
  // Make sure all the pins are set as outputs
#if SPWM_COUNT >= 1
  DDRB |= (1 << SPWM_PIN0);
  PORTB &= ~(1 << SPWM_PIN0);
#endif
#if SPWM_COUNT >= 2
  DDRB |= (1 << SPWM_PIN1);
  PORTB &= ~(1 << SPWM_PIN1);
#endif
#if SPWM_COUNT >= 3
  DDRB |= (1 << SPWM_PIN2);
  PORTB &= ~(1 << SPWM_PIN2);
#endif
#if SPWM_COUNT >= 4
  DDRB |= (1 << SPWM_PIN3);
  PORTB &= ~(1 << SPWM_PIN3);
#endif
  // Make sure ticksInit() gets called to set up the timer
  ticksInit();
  }

/** Set the value of a software PWM pin
 *
 * Set the output for a software PWM pin.
 *
 * @param pwm   the software PWM output to change
 * @param value the value for the duty cycle ranging from 0 (fully off) to 255
 *              (fully on).
 */
void spwmOut(SPWM pwm, uint8_t value) {
  if(pwm<SPWM_COUNT)
    g_pwmout[pwm] = value;
  }

/** Get the current value of a software PWM pin
 *
 * Returns the current value of a software PWM pin.
 *
 * @param pwm the software PWM output to query
 *
 * @return the current value of the software PWM pin
 */
uint8_t spwmValue(SPWM pwm) {
  if(pwm<SPWM_COUNT)
    return g_pwmout[pwm];
  // Invalid pin
  return 0;
  }

#endif /* SOFTPWM_ENABLED */

//---------------------------------------------------------------------------
// Shared interrupt handler
//---------------------------------------------------------------------------

#if defined(SOFTPWM_ENABLED) || defined(SYSTICK_ENABLED)

/** Interrupt handler
 */
ISR(TIMER1_OVF_vect) {
  // Update the 'ticklet' count
  g_ticklet += (256 / TICKLETS);
#ifdef SOFTPWM_ENABLED
  // Update PWM outputs
#  if SPWM_COUNT >= 1
  if((g_pwmout[0]>0)&&(g_ticklet<=g_pwmout[0]))
    pinHigh(SPWM_PIN0);
  else
    pinLow(SPWM_PIN0);
#  endif
#  if SPWM_COUNT >= 2
  if((g_pwmout[1]!=0)&&(g_ticklet<=g_pwmout[1]))
    pinHigh(SPWM_PIN1);
  else
    pinLow(SPWM_PIN1);
#  endif
#  if SPWM_COUNT >= 3
  if((g_pwmout[2]!=0)&&(g_ticklet<=g_pwmout[2]))
    pinHigh(SPWM_PIN2);
  else
    pinLow(SPWM_PIN2);
#  endif
#  if SPWM_COUNT >= 4
  if((g_pwmout[3]!=0)&&(g_ticklet<=g_pwmout[3]))
    pinHigh(SPWM_PIN3);
  else
    pinLow(SPWM_PIN3);
#  endif
#endif /* SOFTPWM_ENABLED */
  if(g_ticklet!=0)
    return;
  // If 'ticklet' wraps around, update the tick count
  g_systicks++;
  }

#endif /* SOFTPWM_ENABLED || SYSTICK_ENABLED */
