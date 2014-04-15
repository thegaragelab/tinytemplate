/*--------------------------------------------------------------------------*
* Analog input helpers
*---------------------------------------------------------------------------*
* 15-Apr-2014 ShaneG
*
* These functions provide simple and accurate analog value reading.
*--------------------------------------------------------------------------*/
#ifndef __ANALOG_H
#define __ANALOG_H

//--- Required definitions
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Initialise the specified analog pin
 *
 * @param pin the analog pin number to initialise.
 */
void analogInit(uint8_t pin);

/** Read a value from the given pin
 *
 * @param pin the analog pin number to read
 * @param skip the number of samples to skip (read and ignore).
 * @param average the number of samples to average (must be >= 1)
 *
 * @return the sample value.
 */
uint16_t analogRead(uint8_t pin, uint8_t skip, uint8_t average);

#ifdef __cplusplus
}
#endif

#endif /* __ANALOG_H */
