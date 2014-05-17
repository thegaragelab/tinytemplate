# Standard Makefile for ATtiny85 projects
#----------------------------------------------------------------------------
# 22-Dec-2013 ShaneG
#
# You will need avr-gcc for this.
#----------------------------------------------------------------------------
BASE_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

# CPU specific options
MCU       = attiny85
F_CPU     = 8000000

# Define tools and options
# All you really need to do is change the TARGET name
TARGET   := attiny85
SIZE     := avr-size
CXX      := avr-gcc
OPTIMISE := -ffunction-sections -fdata-sections -ffreestanding
CFLAGS   := -std=gnu99 -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) -I$(BASE_DIR)/include
LDFLAGS  := -mmcu=$(MCU) -Wl,--gc-sections -Wl,--relax
OBJCOPY  := avr-objcopy
MBFLASH  := tools/mbflash.py
DOXYGEN  := doxygen

# Check for DEBUG options
ifneq ($(DEBUG),)
CFLAGS  += -DDEBUG
endif

# Main source
SOURCES = \
  main.c

# Shared source
SHARED = \
  shared/uart_send.c \
  shared/uart_recv.c \
  shared/uart_print.c \
  shared/uart_format.c \
  shared/utility.c \
  shared/systicks.c \
  shared/crc16.c \
  shared/analog.c \
  shared/pwm.c \
  shared/softspi.c \
  shared/smallfont.c \
  shared/nokialcd.c

# Derive object file names
OBJECTS  = $(filter-out $(SOURCES), $(patsubst %.c,%.o,$(SOURCES)) $(patsubst %.S,%.o,$(SOURCES)))
OBJECTS += $(filter-out $(SHARED), $(patsubst %.c,%.o,$(SHARED)) $(patsubst %.S,%.o,$(SHARED)))

.PHONY: all clean docs

all: $(TARGET).hex

clean:
	@rm -f $(OBJECTS) $(TARGET).hex $(TARGET).elf

flash: $(TARGET).hex
ifneq ($(PORT),)
	@$(MBFLASH) -d $(MCU) -p $(PORT) $(TARGET).hex
else
	@$(MBFLASH) -d $(MCU) $(TARGET).hex
endif

docs:
	@rm -rf docs
	@echo "Generating documentation ..."
	@$(DOXYGEN) doxygen.config
	@mkdir -p html
	@cp -R docs/* html

$(TARGET).elf: $(OBJECTS)
	@echo Linking
	@$(CXX) $(LDFLAGS) -o $@ $(OBJECTS)
	@$(SIZE) --mcu=$(MCU) --format=avr $@

$(TARGET).hex: $(TARGET).elf
	@echo Creating Hex File
	@$(OBJCOPY) -j .text -j .data -O ihex $< $@

# How to compile
%.o: %.c
	@echo Compiling $<
	@$(CXX) $(CFLAGS) $(OPTIMISE) -c $< -o $@

%.o: %.S
	@echo Assembling $<
	@$(CXX) $(CFLAGS) $(OPTIMISE) -c $< -o $@

