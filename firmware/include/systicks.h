/*--------------------------------------------------------------------------*
* System ticks
*---------------------------------------------------------------------------*
* 14-May-2014 ShaneG
*
* Provide access to system 'ticks' - a low resolution timing system.
*--------------------------------------------------------------------------*/
#ifndef __SYSTICKS_H
#define __SYSTICKS_H

//--- Required definitions
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Number of ticks per second
 *
 * Defines the approximate number of ticks that occur every second.
 */
#define TICKS_PER_SECOND 60

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
void ticksInit();

/** Get the current tick count
 *
 * @return the current tick count. The count wraps around once it reaches
 *         65535.
 */
uint16_t ticks();

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
uint16_t ticksElapsed(uint16_t reference);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTICKS_H */
