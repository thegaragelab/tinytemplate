/*--------------------------------------------------------------------------*
* IO convenience functions
*---------------------------------------------------------------------------*
* 16-Apr-2014 ShaneG
*
* These functions provide simple ways to deal with common IO functions.
*--------------------------------------------------------------------------*/
#ifndef __IOHELP_H
#define __IOHELP_H

//--- Required definitions
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Digital helper functions
//---------------------------------------------------------------------------

/** Macro to set a pin high
 *
 * Set the output value of a pin on port B high
 *
 * @param pin the pin number to change
 */
#define pinHigh(pin) \
  PORTB |= (1 << pin)

/** Set a pin low
 *
 * Set the output value of a pin on port B low
 *
 * @param pin the pin number to change
 */
#define pinLow(pin) \
  PORTB &= ~(1 << pin)

/** Set the state of a pin
 *
 * Alternative function to pinHigh/pinLow macros.
 *
 * @param pin the pin number to change
 * @param value the value to set (true = high, false = low)
 */
void pinSet(uint8_t pin, bool value);

/** Get the state of a pin
 *
 * @param pin the pin number to read
 */
#define pinGet(pin) \
  (PINB & (1 << pin))

//---------------------------------------------------------------------------
// Analog helper functions
//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------
// PWM helpers
//---------------------------------------------------------------------------

/** Initialise a PWM pin
 *
 * Set up a PWM output pin.
 *
 * @param pin the pin number to initialise.
 * @param frequency the frequency to operate at.
 */
void pwmInit(uint8_t pin, uint16_t frequency);

/** Set the value of a PWM pin
 *
 * Set the output for a PWM pin.
 *
 * @param pin the pin number of the output to modify.
 * @param value the value for the duty cycle ranging from 0 (fully off) to 255
 *              (fully on).
 */
void pwmOut(uint8_t pin, uint8_t value);

//---------------------------------------------------------------------------
// Software SPI
//---------------------------------------------------------------------------

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
void sspiInit(uint8_t sck, uint8_t mosi, uint8_t miso);

/** Transfer data to a slave (MSB first)
 *
 * @param sck the pin to use for the SCK output
 * @param mosi the pin to use for the MOSI output
 * @param data the data to transfer
 * @param bits the number of bits to transfer
 */
void sspiOutMSB(uint8_t sck, uint8_t mosi, uint16_t data, uint8_t bits);

/** Transfer data from a slave (MSB first)
 *
 * @param sck the pin to use for the SCK output
 * @param miso the pin to use for the MISO input
 * @param bits the number of bits to transfer
 *
 * @return a 16 bit value containing the data in the lowest bits.
 */
uint16_t sspiInMSB(uint8_t sck, uint8_t miso, uint8_t bits);

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
uint16_t sspiInOutMSB(uint8_t sck, uint8_t mosi, uint8_t miso, uint16_t data, uint8_t bits);

/** Transfer data to a slave (LSB first)
 *
 * @param sck the pin to use for the SCK output
 * @param mosi the pin to use for the MOSI output
 * @param data the data to transfer
 * @param bits the number of bits to transfer
 */
void sspiOutLSB(uint8_t sck, uint8_t mosi, uint16_t data, uint8_t bits);

/** Transfer data from a slave (LSB first)
 *
 * @param sck the pin to use for the SCK output
 * @param miso the pin to use for the MISO input
 * @param bits the number of bits to transfer
 *
 * @return a 16 bit value containing the data in the lowest bits.
 */
uint16_t sspiInLSB(uint8_t sck, uint8_t miso, uint8_t bits);

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
uint16_t sspiInOutLSB(uint8_t sck, uint8_t mosi, uint8_t miso, uint16_t data, uint8_t bits);

#ifdef __cplusplus
}
#endif

#endif /* __IOHELP_H */
