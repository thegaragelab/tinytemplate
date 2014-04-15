/*--------------------------------------------------------------------------*
* CRC16 calculations (CCITT standard)
*---------------------------------------------------------------------------*
* 15-Apr-2014 ShaneG
*
* Helper function to generate a 16 bit CRC from binary data. This implement
* is optimised for a small lookup table - saving flash. Code is based on the
* example found here -
*    http://www.digitalnemesis.com/info/codesamples/embeddedcrc16/
*--------------------------------------------------------------------------*/
#include <avr/pgmspace.h>
#include "utility.h"

/** Lookup table
 *
 * This algorithm processes the data in 4 bit chunks so the lookup table only
 * requires 16 entries. We place it in program memory and access it through
 * the pgmspace functions.
 */
static const uint16_t CRC_LOOKUP[] PROGMEM = {
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
  0x0881, 0x2991, 0x4AA1, 0x6BB1, 0x8CC1, 0xADD1, 0xCEE1, 0xEFF1
  };

/** Initialise the CRC calculation
 *
 * Initialises the CRC value prior to processing data.
 *
 * @return the initial CRC value.
 */
uint16_t crcInit() {
  return 0xFFFF; // As per CCITT standard
  }

/** Add a byte to an ongoing CRC calculation
 *
 * Update the CRC value with an additional data byte.
 *
 * @param crc the current CRC value
 * @param data the data byte to add to the calculation
 *
 * @return the updated CRC value.
 */
uint16_t crcByte(uint16_t crc, uint8_t data) {
  uint8_t work, val;
  // Do the high nybble first
  work = crc >> 12;
  val = data >> 4;
  work ^= val;
  crc = crc << 4;
  crc ^= pgm_read_word_near(CRC_LOOKUP + work);
  // Now do the low nybble
  work = crc >> 12;
  val = data & 0x0F;
  work ^= val;
  crc = crc << 4;
  crc ^= pgm_read_word_near(CRC_LOOKUP + work);
  // All done
  return crc;
  }

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
uint16_t crcData(uint16_t crc, const uint8_t *pData, uint8_t length) {
  for(uint8_t index=0; index<length; index++)
    crc = crcByte(crc, pData[index]);
  return crc;
  }

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
uint16_t crcDataP(uint16_t crc, const uint8_t *pData, uint8_t length) {
  for(uint8_t index=0; index<length; index++)
    crc = crcByte(crc, pgm_read_byte_near(pData + index));
  return crc;
  }

