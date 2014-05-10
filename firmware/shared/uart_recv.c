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

// Only if enabled
#ifdef UART_ENABLED

//--- Set up input buffer if applicable
#ifdef UART_INTERRUPT
/** The input buffer for serial input
 *
 * In interrupt driven mode this buffer will hold the characters as they
 * come in.
 */
static uint8_t g_buffer[UART_BUFFER];

/** Current buffer index
 */
static volatile uint8_t g_index = 0;
#endif

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
#ifndef UART_INTERRUPT
  return 0;
#else
  return g_index;
#endif
  }

/** Receive a single character
 *
 * Wait for a single character on the UART and return it.
 *
 * @return the character received.
 */
char uartRecv() {
  char ch;
#ifdef UART_INTERRUPT
  // Wait for a character
  while(g_index==0);
  cli();
  // Return the first character in the buffer
  ch = g_buffer[0];
  g_index--;
  // Move everything down
  for(uint8_t index=0; index<g_index; g_buffer[index] = g_buffer[index + 1], index++);
  // Done
  sei();
#else
  // Set as input and disable pullup
  DDRB  &= ~(1 << UART_RX);
  PORTB &= ~(1 << UART_RX);
  // Read the byte
  cli();
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
    : [uart_port] "I" (_SFR_IO_ADDR(PORTB)),
      [uart_pin] "I" (UART_RX),
      [rxdelay] "I" (RXDELAY),
      [rxdelay2] "I" (RXDELAY2)
    : "r0","r18","r19");
  sei();
#endif
  return ch;
  }

#ifdef UART_INTERRUPT
/* Interrupt handler for the pin change
 */
ISR(PCINT0_vect) {
  uint8_t ch;
  // Make sure it is our pin and it is 0
  if(!(PINB&(1<<UART_RX))) {
    // Start the read (assuming we have the start bit)
    asm volatile(
      "  ldi r18, %[rxdelay2]              \n\t" // 1.5 bit delay
      "  ldi %0, 0x80                      \n\t" // bit shift counter
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
      : [uart_port] "I" (_SFR_IO_ADDR(PORTB)),
        [uart_pin] "I" (UART_RX),
        [rxdelay] "I" (RXDELAY),
        [rxdelay2] "I" (RXDELAY2)
      : "r0","r18","r19");
    // Now put it in the buffer (if we have room)
    if(g_index<UART_BUFFER)
      g_buffer[g_index++] = ch;
    }
  // TODO: Chain on to the user interrupt handler if available.
  }
#endif /* UART_INTERRUPT */

#endif /* UART_ENABLED */
