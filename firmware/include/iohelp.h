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

/** Analog pin inputs
 */
typedef enum _ANALOG {
  ADC0 = 0x00, //!< ADC0, PB5
  ADC1,        //!< ADC1, PB2
  ADC2,        //!< ADC2, PB4
  ADC3,        //!< ADC3, PB3
  ADC4 = 0x0F  //!< ADC4 - internal temperature sensor
  } ANALOG;

/** Initialise the specified analog pin
 *
 * This function initialises the ADC accessory as well as setting up the input
 * pin attached to the ADC. The set up uses VCC as the reference voltage. If
 * you need to change this do so after initialising all the ADC inputs you
 * want to use.
 *
 * @param adc the ADC input to initialise.
 */
void adcInit(ANALOG adc);

/** Read a value from the analog input
 *
 * Read a 10 bit value from the specified input with option input skipping and
 * averaging.
 *
 * @param adc the ADC input to read
 * @param skip the number of samples to skip (read and ignore).
 * @param average the number of samples to average (must be >= 1)
 *
 * @return the sample value.
 */
uint16_t adcRead(ANALOG adc, uint8_t skip, uint8_t average);

//---------------------------------------------------------------------------
// Hardware PWM helpers
//---------------------------------------------------------------------------

/** PWM outputs
 *
 * This enumerates the available PWM outputs.
 */
typedef enum _PWM {
  PWM0 = 0, //!< PWM output 0 (PB0)
  PWM1 = 1, //!< PWM output 1 (PB1)
  } PWM;

/** Initialise the PWM system
 *
 * Sets up PWM support. The library only supports hardware PWM on TIMER0
 * so you only get up to two PWM pins - PB0 and PB1.
 */
void pwmInit();

/** Set the value of a PWM pin
 *
 * Set the output for a PWM pin.
 *
 * @param pwm   the PWM output to change
 * @param value the value for the duty cycle ranging from 0 (fully off) to 255
 *              (fully on).
 */
void pwmOut(PWM pwm, uint8_t value);

//---------------------------------------------------------------------------
// Software PWM implementation
//---------------------------------------------------------------------------

/** Software PWM outputs
 *
 * This enumerates the available outputs for software generated PWM.
 */
typedef enum _SPWM {
  SPWM0 = 0, //!< Software PWM output 0 (defined by SPWM_PIN0)
  SPWM1 = 1, //!< Software PWM output 1 (defined by SPWM_PIN1)
  SPWM2 = 2, //!< Software PWM output 2 (defined by SPWM_PIN2)
  SPWM3 = 3, //!< Software PWM output 3 (defined by SPWM_PIN3)
  } SPWM;

/** Initialise the software PWM system
 *
 * Sets up software PWM support. Using the software PWM implementation also
 * enables the system ticks module (they share the same interrupt).
 */
void spwmInit();

/** Set the value of a software PWM pin
 *
 * Set the output for a software PWM pin.
 *
 * @param pwm   the software PWM output to change
 * @param value the value for the duty cycle ranging from 0 (fully off) to 255
 *              (fully on).
 */
void spwmOut(SPWM pwm, uint8_t value);

/** Get the current value of a software PWM pin
 *
 * Returns the current value of a software PWM pin.
 *
 * @param pwm the software PWM output to query
 *
 * @return the current value of the software PWM pin
 */
uint8_t spwmValue(SPWM pwm);

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
