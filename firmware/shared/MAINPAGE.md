This repository provides a simple template that can be used to develop ATtiny85
based projects. As well as support for a serial bootloader (via the
[Microboot](https://github.com/thegaragelab/microboot) software and tools) and
template [Fritzing](http://fritzing.org/home/) projects for a basic development
board it provides source for a library of commonly used routines. These pages
document that source.

Some notable features of the library are noted here. Please browse through the
full documentation to see other features.

# Software UART

Defined in [softuart.h](@ref softuart.h).

The library provides a software UART implementation that uses a single pin for
both Tx and Rx (with the help of some additional circuitry). The software and
hardware for this design originally came from
[this site](http://nerdralph.blogspot.com.au/2014/01/avr-half-duplex-software-uart.html).

The library provides a simple method of initialising and reading data from the
implemented serial port through the [uartInit()](@ref uartInit), [uartSend()](@ref uartSend)
and [uartRecv()](@ref uartRecv) functions. Additional helper functions have been
provide to print integers (in both hexadecimal and decimal), strings (from RAM
or program memory) and to provide *printf()* style formatted output.

# IO Helper Functions

Defined in [iohelp.h](@ref iohelp.h)

These functions provide a simple method of configuring and using IO features
of the processor including general digital IO, analog input, PWM output and
a software implementation of the SPI protocol.

# Utility Functions

Defined in [utility.h](@ref utility.h)

This is a collection of helper functions to perform various tasks that are not
directly related to IO.

