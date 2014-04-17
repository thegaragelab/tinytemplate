#!/usr/bin/env python
#----------------------------------------------------------------------------
# 04-Mar-2014 ShaneG
#
# Utility to dump the contents of a device.
#----------------------------------------------------------------------------
import sys
from os.path import splitext, basename
from intelhex import IntelHex
from microboot import Microboot, MicrobootException
from mbutil import beginProgress, updateProgress, endProgress

#--- Banner and usage information
BANNER = """
mbdump.py - Microboot/Microboard Firmware Dump Utility
Copyright (c) 2014, The Garage Lab. All Rights Reserved.
"""

USAGE = """
Usage:

  %s options [filename]

Options:
  -d,--device name  Specify the expected device, eg: attiny85,atmega8. This
                    is required.
  -p,--port name    Specify the name of the serial port to use for communication.
                    If not specified the port /dev/ttyUSB0 will be used.
  --log             Log all communications to the file 'transfer.log'

If a filename is not specified the output will be saved in the file 'device'.hex,
eg atmega8.hex if the device is an atmega8.
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
    filename = device + ".hex"
  # Add default extension to filename
  name, ext = splitext(filename)
  if ext == "":
    filename = name + ".hex"
  # Set up the device interface
  mb = Microboot()
  info = mb.getDeviceInfo(device)
  if info is None:
    print "Unsupported device type '%s'." % device
  # Show what we are doing
  size = info[4] - info[3] + 1
  print "Reading %d bytes (0x%04X:0x%04X) from '%s' on '%s'." % (size, info[3], info[4], device, port)
  # Set up logging if requested
  if g_logFile is not None:
    mb.logger = logFunction
  # Connect to the device
  mb.connect(device, port)
  # Read everything
  data = None
  beginProgress("Reading")
  try:
    data = mb.read(info[3], size, updateProgress)
  except Exception, ex:
    endProgress()
    print "Error: Reading failed, error message is:"
    print "       " + str(ex)
    exit(1)
  endProgress()
  mb.disconnect()
  # Create the HEX file
  hexfile = IntelHex()
  address = info[3]
  for val in data:
    hexfile[address] = val
    address = address + 1
  hexfile.tofile(filename, "hex")
  print "Output written to '%s'." % filename

