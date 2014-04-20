# ATtiny85 Project Template

A complete starter solution for ATtiny85 based hardware projects. This project
is released under a [Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0/).

The project provides the following features:

1. Schematics (in [Fritzing](http://fritzing.org/home/) format) for two
   variations of an ATtiny85 breakout board for rapid development. See
   [this post](http://thegaragelab.com/posts/attiny85-breakout-board.html)
   for more information.
2. Hex files and support tools for the [Microboot](https://github.com/thegaragelab/microboot)
   bootloader.
3. A library of support functions for the ATtiny providing features suchs as
   single pin serial communications, '*printf()*' like formatted serial output,
   helper functions for digital and analog pin control as well as a collection
   of useful support functions for common embedded tasks.
4. A *Makefile* configured to use an installation of *avr-gcc* to compile, link
   and flash your application.

The project contains three top level directories, the '*firmware*' directory
contains the *Makefile* and the source for the support library, the '*hardware*'
directory contains the Fritzing project files for the breakout boards and the
*software* directory is reserved for any desktop software that forms part of
your project.

# Creating a New Project

To set up the template for your own project after cloning you simply need to
edit the *Makefile* in the '*firmware*' directory to set your preferred target
file name. For simple projects all you need to do is modify the code in
'*main.c*', if your project requires additional sources you need to add them
to the *SOURCES* definition in the *Makefile*.

# Using the Makefile

Some common usage patterns are:

```shell
# Do a clean build
make clean; make
# Do a debug build (set DEBUG for source files and enables PRINTF(),
# PRINT() debug output functions)
make DEBUG=true
# Flash the code to the chip (by default uses Microboot tools and expects the
# device to be on /dev/ttyUSB0 - change the Makefile for different behaviour)
make flash
```

# Library Status

The support library is an ongoing project and not all functionality is
implemented and tested yet. In some cases planned functionality has been
added to the header files but is not yet implemented. The current status
is:

Feature         |Implemented|Tested
----------------|-----------|------
1 Pin Serial    |Yes        |Yes
Formatted Output|Yes        |Yes
Digital IO      |Mostly     |No
Analog IO       |Yes        |Yes
PWM Control     |No         |No
Software SPI    |Yes        |No

This table will be updated to reflect changes in the implementation and testing
status of the features. Check back to see if what you are looking for has
become available.

