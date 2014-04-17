#!/usr/bin/env python
#----------------------------------------------------------------------------
# 04-Mar-2014 ShaneG
#
# Utility to flash a microcontroller over serial connections.
#----------------------------------------------------------------------------
import sys
from os.path import splitext, basename, exists
from intelhex import IntelHex
from microboot import Microboot, MicrobootException
from mbutil import beginProgress, updateProgress, endProgress

#--- Banner and usage information
BANNER = """
mbflash.py - Microboot/Microboard System Flashing Utility
Copyright (c) 2014, The Garage Lab. All Rights Reserved.
"""

USAGE = """
Usage:

  %s options filename

Options:
  -d,--device name  Specify the expected device, eg: attiny85,atmega8. This
                    is required.
  -p,--port name    Specify the name of the serial port to use for communication.
                    If not specified the port /dev/ttyUSB0 will be used.
  --log             Log all communications to the file 'transfer.log'
"""

#--- Communications log
g_logFile = None

#----------------------------------------------------------------------------
# Helper functions
#----------------------------------------------------------------------------

def showUsage():
  print USAGE.strip() % basename(sys.argv[0])
  exit(1)

def logFunction(send, recv):
  g_logFile.write(">" + send)
  g_logFile.write("<" + recv)
  g_logFile.flush()

def adjustStartup(info, hexfile):
  if hexfile.minaddr() != 0:
    return hexfile # Nothing to do
  print "Adjusting RESET vector for bootloader support ..."
  # Get the upper address
  upper = info[4]
  # We expect the first word to be 'jsr xxxxxx' (0xcxxx)
  opcode = (hexfile[1] << 8) | hexfile[0]
  if (opcode & 0xF000) <> 0xC000:
    raise MicrobootException("Expected 'jsr XXX' (0xCXXX) instruction at start. Got %04x" % opcode)
  # Make sure we have room to stash the address
  if hexfile.maxaddr() >= (upper - 2):
    raise MicrobootException("Application code would overwrite address storage location.")
  # Calculate the application start address
  address = (opcode & 0x0FFF) + 1
  print "Application start : %04x" % address
  # Generate the correct code
  opcode = 0xC000 | ((((upper + 1) / 2) & 0x0FFF) - 1)
  print "Bootloader start : %04x" % ((opcode & 0x0FFF) + 1)
  hexfile[0] = opcode & 0xFF
  hexfile[1] = (opcode >> 8) & 0xFF
  # Store the address at the top of memory
  hexfile[upper] = (address >> 8) & 0xFF
  hexfile[upper - 1] = address & 0xFF
  # All done
  print ""
  return hexfile

#----------------------------------------------------------------------------
# Main program
#----------------------------------------------------------------------------

if __name__ == "__main__":
  print BANNER.strip() + "\n"
  # Set up defaults
  device = None
  port = "/dev/ttyUSB0"
  filename = None
  # Process command line arguments
  index = 1
  while index < len(sys.argv):
    arg = sys.argv[index]
    if (arg == "--device") or (arg == "-d"):
      index = index + 1
      device = sys.argv[index]
    elif (arg == "--port") or (arg == "-p"):
      index = index + 1
      port = sys.argv[index]
    elif (arg == "--log"):
      g_logFile = open("transfer.log", "w")
    else:
      filename = arg
      index = index + 1
      if index <> len(sys.argv):
        print "Error: Filename must be the last parameter.\n"
        showUsage()
    index = index + 1
  # Rationalise parameters
  if device is None:
    print "Error: You must specify a device.\n"
    showUsage()
  if filename is None:
    print "Error: You must specify a filename.\n"
    showUsage()
  # Add default extension to filename if the unadorned version does not exist
  if not exists(filename):
    name, ext = splitext(filename)
    if ext == "":
      filename = name + ".hex"
  if not exists(filename):
    print "Error: You must specify a filename.\n"
    showUsage()
  # Set up the device interface
  mb = Microboot()
  info = mb.getDeviceInfo(device)
  if info is None:
    print "Unsupported device type '%s'." % device
    exit(1)
  # Set up logging if requested
  if g_logFile is not None:
    mb.logger = logFunction
  # Load the HEX file
  hexfile = IntelHex()
  hexfile.fromfile(filename, format='hex')
  if device == "attiny85":
    # Adjust the code to move the RESET vector
    hexfile = adjustStartup(info, hexfile)
  # Set up for the write
  start = hexfile.minaddr()
  length = (hexfile.maxaddr() - start) + 1
  data = list()
  for index in range(length):
    data.append(hexfile[start + index])
  print "Writing %d bytes (%04X:%04X) from file '%s'." % (length, start, start + length - 1, filename)
  print "Target is a '%s' on port '%s'." % (device, port)
  # Write the data
  try:
    mb.connect(device, port)
  except Exception, ex:
    print "Error: Could not connect to device, error message is:"
    print "       " + str(ex)
    exit(1)
  beginProgress("Writing")
  try:
    mb.write(start, length, data, updateProgress)
  except Exception, ex:
    endProgress()
    print "Error: Writing to flash failed, error message is:"
    print "       " + str(ex)
    exit(1)
  endProgress()
  # Now verify
  beginProgress("Verifying")
  try:
    mb.verify(start, length, data, updateProgress)
  except Exception, ex:
    endProgress()
    print "Error: Verification failed, error message is:"
    print "       " + str(ex)
    exit(1)
  endProgress()
  mb.disconnect()

