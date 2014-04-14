/*--------------------------------------------------------------------------*
* Internal configuration for software UART implementation
*---------------------------------------------------------------------------*
* 14-Apr-2014 ShaneG
*
* Provides additional configuration for the software UART implementation
* based on the values set in 'hardware.h'.
*--------------------------------------------------------------------------*/
#ifndef __UART_DEFS_H
#define __UART_DEFS_H

#ifdef F_CPU
  /* account for integer truncation by adding 3/2 = 1.5 */
  #define TXDELAY   (int)(((F_CPU/BAUD_RATE)-7 +1.5)/3)
  #define RXDELAY   (int)(((F_CPU/BAUD_RATE)-5 +1.5)/3)
  #define RXDELAY2  (int)((RXDELAY*1.5)-2.5)
  #define RXROUNDED (((F_CPU/BAUD_RATE)-5 +2)/3)
  #if RXROUNDED > 127
    #error low baud rates unsupported - use higher BAUD_RATE
  #endif
#else
  #error CPU frequency F_CPU undefined
#endif

#endif /* __UART_DEFS_H */
