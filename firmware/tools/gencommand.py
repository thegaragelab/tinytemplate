#!/usr/bin/env python
#----------------------------------------------------------------------------
# 07-Mar-2014 ShaneG
#
# Generate command strings to use while testing the bootloader.
#----------------------------------------------------------------------------
import sys
from os.path import basename
from random import getrandbits
from microboot import Microboot, MicrobootException

#--- Banner and usage information
BANNER = """
gencommand.py - Microboot/Microboard Command Testing Utility
Copyright (c) 2014, The Garage Lab. All Rights Reserved.
"""

USAGE = """
Usage:

  %s options [filename]

Options:
  -d,--device name           Specify the expected device, eg: attiny85,atmega8.
                             This is required.
  -b,--blocksize size        Specify the size of each data block in bytes. The default
                             is 32.
  -c,--command [read|write]  Specify the command sequence to generate.
  -s,--start address         Specify the start address. The default is the
                             devices lowest accessable address.
  -l,--length length         Specify the number of bytes to process. The default
                             is the entire flash memory for the device.
  -r,--random                For the write process only - use random bytes
                             instead of a HEX file.
"""

#----------------------------------------------------------------------------
# Helper functions
#----------------------------------------------------------------------------

def showUsage():
  print USAGE.strip() % basename(sys.argv[0])
  exit(1)

def verifyAddress(info, start, length):
  if (start < info[3]) or (start > info[4]) or ((start + length - 1) > info[4]):
    print "Error: Address out of range for device,"
    print "       You specified %04X:%04X" % (start, start + length - 1)
    print "       Device accepts %04X:%04X" % (info[3], info[4])
    exit(1)

#----------------------------------------------------------------------------
# Main program
#----------------------------------------------------------------------------

if __name__ == "__main__":
  print BANNER.strip() + "\n"
  # Set up defaults
  device = None
  blocksize = 16
  filename = None
  command = None
  start = None
  length = None
  random = False
  # Process command line arguments
  index = 1
  while index < len(sys.argv):
    arg = sys.argv[index]
    if (arg == "--device") or (arg == "-d"):
      index = index + 1
      device = sys.argv[index]
    elif (arg == "--blocksize") or (arg == "-b"):
      index = index + 1
      blocksize = int(sys.argv[index])
    elif (arg == "--command") or (arg == "-c"):
      index = index + 1
      command = sys.argv[index].lower()
    elif (arg == "--start") or (arg == "-s"):
      index = index + 1
      start = int(sys.argv[index], 16)
    elif (arg == "--length") or (arg == "-l"):
      index = index + 1
      length = int(sys.argv[index])
    elif (arg == "--random") or (arg == "-r"):
      random = True
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
  if not command in ("read", "write"):
    print "Error: Command must be one of 'read' or 'write'.\n"
    showUsage()
  if (command == "write") and (filename is None) and (not random):
    print "Error: You must specify an input file or --random with the 'write' command\n"
    showUsage()
  # Show information
  print "Device    : %s" % device
  print "Block size: %d" % blocksize
  print "Operation : %s" % command
  # Set up the interface
  mb = Microboot()
  info = mb.getDeviceInfo(device)
  if info is None:
    print "Error: Unsupported device type '%s'." % device
    exit(1)
  mb.DATA_SIZE = blocksize
  # Do the task
  if command == "read":
    if start is None:
      start = info[3]
    if length is None:
      length = info[4] - start + 1
    # Show data range being read
    print "Start Addr: %04X" % start
    print "Byte count: %04X (%d)" % (length, length)
    # Make sure the range is valid for the device
    verifyAddress(info, start, length)
    # Determine the number of bytes to read (must be multiple of mb.DATA_SIZE)
    alength = length
    if (alength % mb.DATA_SIZE) <> 0:
      alength = (int(length / mb.DATA_SIZE) + 1) * mb.DATA_SIZE
    # Now do the read
    read = 0
    address = start
    while read < alength:
      print mb.createReadCommand(address)[:-1]
      address = address + mb.DATA_SIZE
      read = read + mb.DATA_SIZE
  elif command == "write":
    data = list()
    if random:
      # Generate random data
      if start is None:
        start = info[3]
      if length is None:
        length = info[4] - info[3] + 1
      verifyAddress(info, start, length)
      for index in range(length):
        data.append(getrandbits(8))
    else:
      # Load the HEX file
      hexfile = IntelHex()
      hexfile.fromfile(filename, format='hex')
      start = hexfile.minaddr()
      length = (hexfile.maxaddr() - start) + 1
      verifyAddress(info, start, length)
      for index in range(length):
        data.append(hexfile[start + index])
    # Determine the number of bytes to write (must be multiple of mb.DATA_SIZE)
    alength = length
    if (alength % mb.DATA_SIZE) <> 0:
      alength = (int(length / mb.DATA_SIZE) + 1) * mb.DATA_SIZE
    while len(data) < alength:
      data.append(0xFF)
    # Generate the write commands
    written = 0
    address = start
    while written < alength:
      print mb.createWriteCommand(address, data, written)[:-1]
      address = address + mb.DATA_SIZE
      written = written + mb.DATA_SIZE


