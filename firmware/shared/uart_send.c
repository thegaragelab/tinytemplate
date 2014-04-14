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

/** Initialise the UART
 */
void uartInit() {
  // Set as input and disable pullup
  UART_DDR  &= ~(1 << UART_PIN);
  UART_PORT &= ~(1 << UART_PIN);
  }

/** Write a single character
 *
 * Send a single character on the UART.
 *
 * @param ch the character to send.
 */
void uartSend(char ch) {
  // Set to output state and bring high
  UART_PORT |= (1 << UART_PIN);
  UART_DDR  |= (1 << UART_PIN);
  asm volatile(
    "  cbi %[uart_port], %[uart_pin]    \n\t"  // start bit
    "  in r0, %[uart_port]              \n\t"
    "  ldi r30, 3                       \n\t"  // stop bit + idle state
    "  ldi r28, %[txdelay]              \n\t"
    "TxLoop:                            \n\t"
    // 8 cycle loop + delay - total = 7 + 3*r22
    "  mov r29, r28                     \n\t"
    "TxDelay:                           \n\t"
    // delay (3 cycle * delayCount) - 1
    "  dec r29                          \n\t"
    "  brne TxDelay                     \n\t"
    "  bst %[ch], 0                     \n\t"
    "  bld r0, %[uart_pin]              \n\t"
    "  lsr r30                          \n\t"
    "  ror %[ch]                        \n\t"
    "  out %[uart_port], r0             \n\t"
    "  brne TxLoop                      \n\t"
    :
    : [uart_port] "I" (_SFR_IO_ADDR(UART_PORT)),
      [uart_pin] "I" (UART_PIN),
      [txdelay] "I" (TXDELAY),
      [ch] "r" (ch)
    : "r0","r28","r29","r30");
  // Change back to idle state
  UART_DDR  &= ~(1 << UART_PIN);
  UART_PORT &= ~(1 << UART_PIN);
  }

