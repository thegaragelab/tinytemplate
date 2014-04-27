/*--------------------------------------------------------------------------*
* Software SPI
*---------------------------------------------------------------------------*
* 19-Apr-2014 ShaneG
*
* An implementation of SPI in software. Useful for LCD displays and other
* devices where a full hardware SPI implementation would waste IO pins.
*--------------------------------------------------------------------------*/
#include <avr/io.h>
#include "iohelp.h"

/** Initialise pins for software SPI
 *
 * It is not necessary to call this in order to use software SPI, it is simply
 * a convenience function to do the necessary setup. You can initialise the
 * pins directly if needed.
 *
 * @param sck the pin to use for the SCK output
 * @param mosi the pin to use for the MOSI output
 * @param miso the pin to use for the MISO input
 */
void sspiInit(uint8_t sck, uint8_t mosi, uint8_t miso) {
  // Set SCK and MOSI to output and bring them both low
  uint8_t val = (1 << sck) | (1 << mosi);
  PORTB &= ~val;
  DDRB |= val;
  // Set MISO to input, no pullup
  val = ~(1 << mosi);
  PORTB &= val;
  DDRB &= val;
  }

/** Transfer data to a slave (MSB first)
 *
 * @param sck the pin to use for the SCK output
 * @param mosi the pin to use for the MOSI output
 * @param data the data to transfer
 * @param bits the number of bits to transfer
 */
void sspiOutMSB(uint8_t sck, uint8_t mosi, uint16_t data, uint8_t bits) {
  uint16_t mask = (1 << (bits - 1));
  uint8_t output = (1 << mosi);
  uint8_t clock = (1 << sck);
  while(bits) {
    // Set data
    if(data&mask)
      PORTB |= output;
    else
      PORTB &= ~output;
    // Bring the clock high
    PORTB |= clock;
    // Move to the next bit
    mask = mask >> 1;
    bits--;
    // Bring the clock low again
    PORTB &= ~clock;
    }
  }

/** Transfer data from a slave (MSB first)
 *
 * @param sck the pin to use for the SCK output
 * @param miso the pin to use for the MISO input
 * @param bits the number of bits to transfer
 *
 * @return a 16 bit value containing the data in the lowest bits.
 */
uint16_t sspiInMSB(uint8_t sck, uint8_t miso, uint8_t bits) {
  uint16_t result = 0;
  uint8_t input = (1 << miso);
  uint8_t clock = (1 << sck);
  while(bits) {
    // Bring the clock high
    PORTB |= clock;
    // Read the next bit
    result = result << 1;
    if(PINB&input)
      result |= 0x01;
    // Bring the clock low again
    PORTB &= ~clock;
    }
  return result;
  }

/** Transfer data to and from a slave (MSB first)
 *
 * @param sck the pin to use for the SCK output
 * @param mosi the pin to use for the MOSI output
 * @param miso the pin to use for the MISO input
 * @param data the data to transfer out
 * @param bits the number of bits to transfer
 *
 * @return a 16 bit value containing the read data in the lowest bits.
 */
uint16_t sspiInOutMSB(uint8_t sck, uint8_t mosi, uint8_t miso, uint16_t data, uint8_t bits) {
  uint16_t result = 0;
  uint16_t mask = (1 << bits);
  uint8_t input = (1 << miso);
  uint8_t output = (1 << mosi);
  uint8_t clock = (1 << sck);
  while(bits) {
    // Set data
    if(data&mask)
      PORTB |= output;
    else
      PORTB &= ~output;
    // Bring the clock high
    PORTB |= clock;
    // Move to the next bit
    mask = mask >> 1;
    bits--;
    // Read the next bit
    result = result << 1;
    if(PINB&input)
      result |= 0x01;
    // Bring the clock low again
    PORTB &= ~clock;
    }
  return result;
  }

/** Transfer data to a slave (LSB first)
 *
 * @param sck the pin to use for the SCK output
 * @param mosi the pin to use for the MOSI output
 * @param data the data to transfer
 * @param bits the number of bits to transfer
 */
void sspiOutLSB(uint8_t sck, uint8_t mosi, uint16_t data, uint8_t bits) {
  uint8_t output = (1 << mosi);
  uint8_t clock = (1 << sck);
  while(bits) {
    // Set data
    if(data&0x01)
      PORTB |= output;
    else
      PORTB &= ~output;
    // Bring the clock high
    PORTB |= clock;
    // Move to the next bit
    data = data >> 1;
    bits--;
    // Bring the clock low again
    PORTB &= ~clock;
    }
  }

/** Transfer data from a slave (LSB first)
 *
 * @param sck the pin to use for the SCK output
 * @param miso the pin to use for the MISO input
 * @param bits the number of bits to transfer
 *
 * @return a 16 bit value containing the data in the lowest bits.
 */
uint16_t sspiInLSB(uint8_t sck, uint8_t miso, uint8_t bits) {
  uint16_t result = 0;
  uint8_t input = (1 << miso);
  uint8_t clock = (1 << sck);
  for(uint8_t count=0; count<bits; count++) {
    // Bring the clock high
    PORTB |= clock;
    // Read the next bit
    result = result >> 1;
    if(PINB&input)
      result |= 0x8000;
    // Bring the clock low again
    PORTB &= ~clock;
    }
  result = result >> (16 - bits);
  return result;
  }

/** Transfer data to and from a slave (LSB first)
 *
 * @param sck the pin to use for the SCK output
 * @param mosi the pin to use for the MOSI output
 * @param miso the pin to use for the MISO input
 * @param data the data to transfer out
 * @param bits the number of bits to transfer
 *
 * @return a 16 bit value containing the read data in the lowest bits.
 */
uint16_t sspiInOutLSB(uint8_t sck, uint8_t mosi, uint8_t miso, uint16_t data, uint8_t bits) {
  uint16_t result = 0;
  uint8_t input = (1 << miso);
  uint8_t output = (1 << mosi);
  uint8_t clock = (1 << sck);
  for(uint8_t count=0; count<bits; count++) {
    // Set data
    if(data&0x01)
      PORTB |= output;
    else
      PORTB &= ~output;
    // Bring the clock high
    PORTB |= clock;
    // Read the next bit
    data = data >> 1;
    result = result >> 1;
    if(PINB&input)
      result |= 0x8000;
    // Bring the clock low again
    PORTB &= ~clock;
    }
  result = result >> (16 - bits);
  return result;
  }

