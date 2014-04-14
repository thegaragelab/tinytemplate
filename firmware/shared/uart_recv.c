/*--------------------------------------------------------------------------*
* UART interface implementation for ATmega
*---------------------------------------------------------------------------*
* 01-Apr-2014 ShaneG
*
* Half-duplex 8N1 serial UART in software.
*
* 1%/2% Tx/Rx timing error for 115.2kbps@8Mhz
* 2%/1% Tx/Rx timing error for 230.4kbps@8Mhz
*
* Uses only one pin on the AVR for both Tx and Rx:
*
*              D1
*  AVR ----+--|>|-----+----- Tx
*          |      10K $ R1
*          +--------(/^\)--- Rx
*               NPN E   C
*--------------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../hardware.h"
#include "uart_defs.h"
#include "softuart.h"

/** Determine if characters are available
 *
 * Check the number of characters available for immediate reading. If this
 * function returns a non-zero value the next call to uartRecv() will be
 * guaranteed to return immediately with a value. This function is only valid
 * if you are using the interrupt driven version of the UART.
 *
 * @return the number of characters available in the input buffer.
 */
uint8_t uartAvail() {
  return 0;
  }

/** Receive a single character
 *
 * Wait for a single character on the UART and return it.
 *
 * @return the character received.
 */
char uartRecv() {
  char ch;
  // Set as input and disable pullup
  UART_DDR  &= ~(1 << UART_PIN);
  UART_PORT &= ~(1 << UART_PIN);
  // Read the byte
  asm volatile(
    "  ldi r18, %[rxdelay2]              \n\t" // 1.5 bit delay
    "  ldi %0, 0x80                      \n\t" // bit shift counter
    "WaitStart:                          \n\t"
    "  sbic %[uart_port]-2, %[uart_pin]  \n\t" // wait for start edge
    "  rjmp WaitStart                    \n\t"
    "RxBit:                              \n\t"
    // 6 cycle loop + delay - total = 5 + 3*r22
    // delay (3 cycle * r18) -1 and clear carry with subi
    "  subi r18, 1                       \n\t"
    "  brne RxBit                        \n\t"
    "  ldi r18, %[rxdelay]               \n\t"
    "  sbic %[uart_port]-2, %[uart_pin]  \n\t" // check UART PIN
    "  sec                               \n\t"
    "  ror %0                            \n\t"
    "  brcc RxBit                        \n\t"
    "StopBit:                            \n\t"
    "  dec r18                           \n\t"
    "  brne StopBit                      \n\t"
    : "=r" (ch)
    : [uart_port] "I" (_SFR_IO_ADDR(UART_PORT)),
      [uart_pin] "I" (UART_PIN),
      [rxdelay] "I" (RXDELAY),
      [rxdelay2] "I" (RXDELAY2)
    : "r0","r18","r19");
  return ch;
  }
