/*--------------------------------------------------------------------------*
* Hardware definitions
*---------------------------------------------------------------------------*
* 14-Apr-2014 ShaneG
*
* Defines the hardware pins to use for parts of the shared library.
*--------------------------------------------------------------------------*/
#ifndef __HARDWARE_H
#define __HARDWARE_H

// Bring in what we need for pin definitions
#include <avr/io.h>

//---------------------------------------------------------------------------
// Feature selection
//
// These defines control what code gets included in the library, make sure
// everything you want is uncommented. Further down the file you get to
// configure the pin assignments and other options for each feature.
//---------------------------------------------------------------------------

/** Software UART
 *
 * This feature provides a software implementation of a UART allowing you to
 * perform serial communications. It supports speeds of up to 250Kbps, can be
 * configured to use a single IO pin and has an option to be interrupt driven.
 */
#define UART_ENABLED

/** System Ticks
 *
 * Enable the system ticks implementation. This allows measurement of longer
 * periods of time (up to 11 minutes) with a 0.01 second precision.
 */
#define SYSTICK_ENABLED

/** Software PWM
 *
 * This enables the software PWM implementation which can generate PWM output
 * at 100Hz on up to 4 output pins.
 */
#define SOFTPWM_ENABLED

//---------------------------------------------------------------------------
// Software UART configuration
//---------------------------------------------------------------------------

/** Baud rate to use
 *
 * The implementation is optimised for higher baudrates - please don't use
 * anything below 57600 on an 8MHz clock. It does work at up to 250000 baud
 * but you may experience a small amount of dropped packets at that speed.
 */
#define BAUD_RATE 57600

/** Define the pin to use for transmission
 */
#define UART_TX   PINB5

/** Define the pin to use for receiving
 *
 * If this pin is the same as the TX pin the code for the single pin UART
 * implementation is compiled. This means no buffering and no interrupts.
 */
#define UART_RX   PINB5

/** Enable interrupt driven mode
 *
 * Only valid in two pin configuration. If this is defined and you are using
 * two pins for the UART then received data will be read and buffered as it
 * arrives.
 */
#define UART_INTERRUPT

/** Size of input buffer
 *
 * Only available in interrupt driven mode. This sets the size of the receive
 * buffer (max 256 bytes).
 */
#define UART_BUFFER 4

//---------------------------------------------------------------------------
// Software PWM configuration
//---------------------------------------------------------------------------

/** Number of software PWM pins required
 *
 * There can be up to 4 pins. For each pin you must define SPWM_PINn where
 * n is SPWM number.
 */
#define SPWM_COUNT 3

/** Pin associated with SPWM0 */
#define SPWM_PIN0 PINB0

/** Pin associated with SPWM1 */
#define SPWM_PIN1 PINB1

/** Pin associated with SPWM2 */
#define SPWM_PIN2 PINB2

/** Pin associated with SPWM3 */
#define SPWM_PIN3 PINB3

//---------------------------------------------------------------------------
// Nokia LCD device support
//
// To use this device uncomment the LCD_ENABLED line below and define your
// pins with the LCD_xxx defines
//---------------------------------------------------------------------------

// Enable Nokia LCD support
//#define LCD_ENABLED

// Nokia LCD pin numbers
#define LCD_MOSI  PINB2
#define LCD_SCK   PINB4
#define LCD_CD    PINB1
#define LCD_RESET PINB0

#endif /* __HARDWARE_H */
