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

/** An inaccurate delay function
 *
 * This function will delay for the given number of milliseconds. This is not
 * an accurate delay (interrupt activity will interfere with the timing and it
 * uses instruction timing approximations to consume the time) but should be
 * close enough for many purposes without having to revert to a timer interrupt.
 */
void wait(uint16_t millis);

/** Convert the low four bits of a byte value into an upper case hex digit.
 *
 * @param value the value to convert
 *
 * @return the ASCII code for the appropriate hex digit.
 */
char hexChar(uint8_t value);

/** Initialise the CRC calculation
 *
 * Initialises the CRC value prior to processing data.
 *
 * @return the initial CRC value.
 */
uint16_t crcInit();

/** Add a byte to an ongoing CRC calculation
 *
 * Update the CRC value with an additional data byte.
 *
 * @param crc the current CRC value
 * @param data the data byte to add to the calculation
 *
 * @return the updated CRC value.
 */
uint16_t crcByte(uint16_t crc, uint8_t data);

/** Add a block of data to an ongoing CRC calculation
 *
 * Add a sequence of bytes from a buffer in RAM. Note that this function can
 * process up to 255 bytes in a single call (which is usually enough for a
 * microcontroller application).
 *
 * @param crc the current CRC value
 * @param pData pointer to the memory buffer
 * @param length the number of bytes to process.
 *
 * @return the updated CRC value.
 */
uint16_t crcData(uint16_t crc, const uint8_t *pData, uint8_t length);

/** Add a block of data to an ongoing CRC calculation
 *
 * Add a sequence of bytes from a buffer in PROGMEM. Note that this function
 * can process up to 255 bytes in a single call (which is usually enough for a
 * microcontroller application).
 *
 * @param crc the current CRC value
 * @param pData pointer to the memory location.
 * @param length the number of bytes to process.
 *
 * @return the updated CRC value.
 */
uint16_t crcDataP(uint16_t crc, const uint8_t *pData, uint8_t length);

#ifdef __cplusplus
  }
#endif

#endif /* __UTILITY_H */
