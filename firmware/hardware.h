/*--------------------------------------------------------------------------*
* Hardware definitions
*---------------------------------------------------------------------------*
* 14-Apr-2014 ShaneG
*
* Defines the hardware pins to use for parts of the shared library.
*--------------------------------------------------------------------------*/
#ifndef __HARDWARE_H
#define __HARDWARE_H

// Bring in IO definitions
#include <avr/io.h>

// Baud rate to use
#define BAUD_RATE 57600

// Port and pin number to use (ATtiny only)
#define UART_PORT PORTB
#define UART_DDR  DDRB
#define UART_PIN  PINB5

// Nokia LCD pin numbers
#define LCD_MOSI  PINB2
#define LCD_SCK   PINB3
#define LCD_CD    PINB4
#define LCD_RESET PINB5

#endif /* __HARDWARE_H */
