#!/usr/bin/env python
#----------------------------------------------------------------------------
# 04-Mar-2014 ShaneG
#
# Support library for the Microboot protocol
#----------------------------------------------------------------------------
import collections

# We need PySerial
try:
  import serial
except:
  print "Error: This tool requires the pySerial module. Please install it."
  exit(1)

#--- Supported chips
CHIPLIST = {
  # Chip         Type  Model Protocol AddrL   AddrH
  "attiny85":  ( 0x01, 0x01, 0x10,    0x0000, 0x1BFF ),
  "atmega8":   ( 0x01, 0x02, 0x10,    0x0000, 0x1BFF ),
  "atmega88":  ( 0x01, 0x03, 0x10,    0x0000, 0x1BFF ),
  "atmega168": ( 0x01, 0x04, 0x10,    0x0000, 0x3BFF ),
  }

#--- These chips have single bit software UARTS, account for echo
SOFTUART = (
  "attiny85",
  )

#--- Protocol constants
CMD_READ      = 'R'    # Character for the READ command
CMD_WRITE     = 'W'    # Character for the WRITE command
CMD_QUERY     = '?'    # Character for the QUERY command
CMD_RESET     = '!'    # Character for the RESET command
STATUS_OK     = '+'    # Character for OK status
STATUS_FAIL   = '-'    # Character for FAIL status
EOL           = '\n'   # Character to use for line termination
CHECKSUM_SEED = 0x5050 # Seed value for calculating checksum

#--- Default values
DEFAULT_SPEED   = 57600  # Baud rate for serial connection
DEFAULT_RETRIES = 3      # Number of retries per command

#----------------------------------------------------------------------------
# Public API and classes
#----------------------------------------------------------------------------

class MicrobootException(Exception):
  """ Used to report protocol and connection related errors.
  """

  def __init__(self, message):
    """ Constructor with a descriptive message

        @param message the descriptive message for the exception.
    """
    self.message = message

  def __str__(self):
    """ Represent this exception as a string

        @return a string representation of the exception
    """
    return "Microboot Exception: " + self.message

class Microboot:
  """ Main helper class for the Microboot flash loader client
  """

  def __init__(self):
    # Set initial state
    self.deviceName = None
    self.deviceInfo = None
    self.bootInfo = None
    self.serial = None
    self.logger = None
    self.DATA_SIZE = None

  #--------------------------------------------------------------------------
  # Helper methods
  #--------------------------------------------------------------------------

  def makeArray(self, data):
    """ Ensure the data parameter is actually a sequence of integers

        @param data the data value to convert to a sequence

        @return the parameter as a sequence.
    """
    if not isinstance(data, collections.Sequence):
      if type(data) == str:
        data = [ chr(x) for x in data ]
      else:
        data = ( data, )
    return data

  def verifyRange(self, data, offset = 0, length = -1):
    """ Verify that the sequence is large enough

        @param data the sequence of values to verify
        @param offset the offset into the array to start reading data
        @param length the number of bytes. If this value is less than zero the
                      remainder of the array will be used.

        @throws MicrobootException if the sequence is not large enough.
    """
    # Rationalise the length
    if length < 0:
      length = len(data) - offset
    # Make sure there is enough data
    if offset > len(data):
      raise MicrobootException("Data array is not of sufficient size (%d > %d)." % (offset, len(data)))
    if (length + offset) > len(data):
      raise MicrobootException("Data array is not of sufficient size (%d > %d)." % (length + offset, len(data)))

  def checksum(self, total, data, offset = 0, length = -1):
    """ Add the given data to the checksum

        @param total the current checksum total
        @param data a sequence of bytes or a single integer value.
        @param offset the offset into the data array to start adding
        @param length the number of bytes to add (-1 for all)

        @return the new checksum

        @throws MicrobootException if the array does not contain enough data
                for the parameters provided or one of the values in the array
                is out of range.
    """
    # Make sure we have a sequence
    data = self.makeArray(data)
    # Adjust the length if required
    if length < 0:
      length = len(data) - offset
    # Make sure there is enough data
    self.verifyRange(data, offset, length)
    # Now update the checksum
    for val in data[offset:offset + length]:
      val = int(val)
      if (val < 0) or (val > 255):
        raise MicrobootException("Byte value is out of range (%d)." % val)
      total = (total + val) & 0xFFFF
    # Done
    return total

  def createWriteCommand(self, address, data, offset = 0):
    """ Create the 'WRITE' command string
    """
    # Make sure we have a sequence
    data = self.makeArray(data)
    # Make sure there is enough data
    self.verifyRange(data, offset, self.DATA_SIZE)
    # Calculate the checksum first
    check = CHECKSUM_SEED
    check = self.checksum(check, ((address >> 8) & 0xFF, address & 0xFF))
    check = self.checksum(check, data, offset, self.DATA_SIZE)
    # Now build up the string and return it
    return "%c%02X%02X%s%02X%02X%c" % (
      CMD_WRITE,
      (address >> 8) & 0xFF, # Address high byte
      address & 0xFF, # Address low byte
      "".join([ "%02X" % val for val in data[offset:offset + self.DATA_SIZE] ]),
      (check >> 8) & 0xFF, # Checksum high byte
      check & 0xFF, # Checksum low byte
      EOL
      )

  def createReadCommand(self, address):
    """ Create the 'READ' command string
    """
    # Calculate the checksum first
    check = CHECKSUM_SEED
    check = self.checksum(check, ((address >> 8) & 0xFF, address & 0xFF))
    # Now build up the string and return it
    return "%c%02X%02X%02X%02X%c" % (
      CMD_READ,
      (address >> 8) & 0xFF, # Address high byte
      address & 0xFF, # Address low byte
      (check >> 8) & 0xFF, # Checksum high byte
      check & 0xFF, # Checksum low byte
      EOL
      )

  def parseData(self, response):
    """ Parse a response string into a sequence of bytes.
    """
    values = list()
    for index in range((len(response) - 2) / 2):
      values.append(int(response[1 + (2 * index):3 + (2 * index)], 16))
    # Verify checksum
    check = CHECKSUM_SEED
    for val in values[:-2]:
      check = self.checksum(check, val)
    if (((check >> 8) & 0xFF) <> values[len(values) - 2]) or ((check & 0xFF) <> values[len(values) - 1]):
      raise MicrobootException("Checksum does not match - expected 0x%04X, got 0x%04X." % (check, (values[len(values) - 2] << 8) | values[len(values) - 1]))
    return values

  def sendCommand(self, command):
    """ Send a command string and read the response
    """
    attempts = 0
    while attempts < DEFAULT_RETRIES:
      # Send the command
      if self.deviceName in SOFTUART:
        # Consume what we send - it gets echoed back
        for ch in command:
          self.serial.write(ch)
          rcvd = self.serial.read(1)
          if rcvd <> ch:
            raise MicrobootException("Unexpected response on serial port - got '%c', expected '%c'." % (chr(rcvd), chr(ch)))
      else:
        written = 0
        while written < len(command):
          written = written + self.serial.write(command[written:])
      # Read the response
      response = ""
      while True:
        data = self.serial.read(1)
        if len(data) == 0:
          break
        response = response + data
        if data == EOL:
          break
      # Do some basic validation on the response
      if len(response) >= 2:
        if response[-1:] == EOL:
          # Provide an option to log it
          if self.logger is not None:
            self.logger(command, response)
          # Do final input processing
          if response[0] == STATUS_OK:
            if len(response) == 2: # OK only
              return ()
            # Turn it into a sequence of bytes (and verify checksum)
            try:
              return self.parseData(response)
            except:
              pass
      # Try again
      attempts = attempts + 1
    # At this stage, we've failed
    raise MicrobootException("Failed to send command '%c', no response or command failed." % command[0])

  #--------------------------------------------------------------------------
  # Public methods
  #--------------------------------------------------------------------------

  def connect(self, device, port, speed = DEFAULT_SPEED):
    """ Connect to the device attached to the given port.

        This will fail if there is a communication error on the port (or the
        port does not exist), the device is not supported or the device on the
        other end of the connection is not the expected device.

        @param port the name of the serial port to connect on.
        @param device the name of the device to expect.

        @return the device information tuple (@see getInfo)

        @throws MicrobootException if the connection attempt failed.
    """
    # If we are already connect, disconnect
    self.disconnect()
    # Verify the device type
    device = device.lower()
    if not CHIPLIST.has_key(device):
      raise MicrobootException("Unrecognised device type '%s'" % device)
    self.deviceName = device
    # Set up the serial port
    self.serial = serial.Serial(
      port = port,
      baudrate = speed,
      timeout = 0.2 # TODO: Should probably be configurable
      )
    # Get device data and verify
    info = self.getBootInfo()
    if info[0] < CHIPLIST[device][2]:
      self.disconnect()
      raise MicrobootException("Bootloader protocol is not supported. Wanted %d, got %d." % (CHIPLIST[device][2], info[0]))
    if (info[2] <> CHIPLIST[device][0]) or (info[3] <> CHIPLIST[device][1]):
      self.disconnect()
      raise MicrobootException("Unexpected processor type - wanted %02X/%02X, got %02X/%02X." % (CHIPLIST[device][0], CHIPLIST[device][1], info[1], info[2]))
    # Set up state
    self.deviceName = device
    self.deviceInfo = CHIPLIST[device]
    # Done
    return self.bootInfo

  def connectEx(self, device, stream):
    """ Connect to the device attached to the given port.

        This will fail if there is a communication error on the port (or the
        port does not exist), the device is not supported or the device on the
        other end of the connection is not the expected device.

        @param port the name of the serial port to connect on.
        @param device the name of the device to expect.

        @return the device information tuple (@see getInfo)

        @throws MicrobootException if the connection attempt failed.
    """
    # If we are already connect, disconnect
    self.disconnect()
    # Verify the device type
    device = device.lower()
    if not CHIPLIST.has_key(device):
      raise MicrobootException("Unrecognised device type '%s'" % device)
    # Use the provided stream
    self.serial = stream
    # Get device data and verify
    info = self.getBootInfo()
    if info[0] < CHIPLIST[device][2]:
      self.disconnect()
      raise MicrobootException("Bootloader protocol is not supported. Wanted %d, got %d." % (CHIPLIST[device][2], info[0]))
    if (info[2] <> CHIPLIST[device][0]) or (info[3] <> CHIPLIST[device][1]):
      self.disconnect()
      raise MicrobootException("Unexpected processor type - wanted %02X/%02X, got %02X/%02X." % (CHIPLIST[device][0], CHIPLIST[device][1], info[1], info[2]))
    # Set up state
    self.deviceName = device
    self.deviceInfo = CHIPLIST[device]
    # Done
    return self.bootInfo

  def connected(self):
    """ Determine if we are connected to a device

        @return True if a connection is currently active.
    """
    return self.serial is not None

  def disconnect(self):
    """ Disconnect from the device.

        Close the connection to the device (and the underlying serial port). If
        the connection is not established yet this method will have no effect.
    """
    if self.connected():
      # Close the connection
      try:
        self.serial.close()
      except:
        pass # Do nothing
    # Restore initial state
    self.deviceName = None
    self.deviceInfo = None
    self.bootInfo = None
    self.serial = None

  def getDeviceInfo(self, device = None):
    """ Get device information
    """
    if device is None:
      return self.deviceInfo
    # Look it up
    device = device.lower()
    if CHIPLIST.has_key(device):
      return CHIPLIST[device]
    return None

  def getBootInfo(self):
    """ Get the bootloader information tuple.

        This method queries the device for it's information block which includes
        protocol version, processor family and processor type.

        @return a 3-tuple of integers consisting of protocol, family and model
                or None if a connection has not been established.
    """
    if not self.connected():
      return None
    # Return cached values if we have them
    if self.bootInfo is None:
      # Query the device for the information
      response = self.sendCommand("%c%c" % (CMD_QUERY, EOL))
      if len(response) <> 6: # 4 data bytes, 2 checksum bytes
        raise MicrobootException("Invalid response from device.")
      self.bootInfo = tuple(response[:4])
      self.DATA_SIZE = self.bootInfo[1]
    return self.bootInfo

  def read(self, start, length, callback = None):
    """ Read data from the flash memory

        Query the chip for the contents of flash starting at 'start' for 'length'
        bytes.

        @param start the start address to read from. Must be a positive integer
                     in the allowable address range for the device.
        @param length the number of bytes to read. Must be a positive integer
                      greater than zero such that start + length is in the valid
                      address range for the device.
        @param callback optional callback function used to report progress. It
                        will be passed two integer values - the number of bytes
                        that have been transferred and the number of bytes
                        requested.


        @return an array of bytes representing the contents of the flash memory
                or None if the device is not connected.

        @throws MicrobootException if the address is out of range or a
                communication error occurs.
    """
    if not self.connected():
      return None
    # Check parameters
    if (start < 0) or (length < 0):
      raise MicrobootException("Parameters out of range - must be positive integers.")
    # Determine the number of bytes to read (must be multiple of self.DATA_SIZE)
    alength = length
    if (alength % self.DATA_SIZE) <> 0:
      alength = (int(length / self.DATA_SIZE) + 1) * self.DATA_SIZE
    # Make sure the addresses are valid for the device
    chipInfo = CHIPLIST[self.deviceName]
    if (start < chipInfo[3]) or (start > chipInfo[4]) or ((start + length - 1) > chipInfo[4]):
      raise MicrobootException("Address out of range for device - %04X:%04X" % (chipInfo[3], chipInfo[4]))
    # Now do the read
    read = 0
    address = start
    result = list()
    while read < alength:
      response = self.sendCommand(self.createReadCommand(address))
      if len(response) <> (self.DATA_SIZE + 4): # Address + data + checksum
        raise MicrobootException("Invalid response from device.")
      result.extend(response[2:-2])
      address = address + self.DATA_SIZE
      read = read + self.DATA_SIZE
      if callback is not None:
        callback(min(read, length), length)
    # All done
    return result[:length]

  def write(self, start, length, data, callback = None):
    """ Write data to the flash memory

        Replace the contents of the flash memory with the given bytes starting
        at address 'start' for 'length' bytes.

        @param start the start address to read from. Must be a positive integer
                     in the allowable address range for the device.
        @param length the number of bytes to read. Must be a positive integer
                      greater than zero such that start + length is in the valid
                      address range for the device.
        @param data an array of bytes to be written to the flash. The length of
                    the must be at least 'length' bytes.
        @param callback optional callback function used to report progress. It
                        will be passed two integer values - the number of bytes
                        that have been transferred and the number of bytes
                        requested.

        @throws MicrobootException if the address is out of range or a
                communication error occurs.
    """
    if not self.connected():
      return
    # Check parameters
    if (start < 0) or (length < 0):
      raise MicrobootException("Parameters out of range - must be positive integers.")
    # Make sure we have an array of data
    data = self.makeArray(data)
    self.verifyRange(data, 0, length)
    # Determine the number of bytes to write (must be multiple of self.DATA_SIZE)
    alength = length
    if (alength % self.DATA_SIZE) <> 0:
      alength = (int(length / self.DATA_SIZE) + 1) * self.DATA_SIZE
    # Make sure the addresses are valid for the device
    chipInfo = CHIPLIST[self.deviceName]
    if (start < chipInfo[3]) or (start > chipInfo[4]) or ((start + length - 1) > chipInfo[4]):
      raise MicrobootException("Address out of range for device - %04X:%04X" % (chipInfo[3], chipInfo[4]))
    # If we have been given less than self.DATA_SIZE bytes to write, pad with what
    # is already there
    if alength <> length:
      current = self.read(start + alength - self.DATA_SIZE, self.DATA_SIZE)
      data.extend(current[self.DATA_SIZE - (alength - length):])
    # Now write the data
    written = 0
    address = start
    while written < alength:
      response = self.sendCommand(self.createWriteCommand(address, data, written))
      if len(response) <> 0: # Should be no data in response
        raise MicrobootException("Invalid response from device.")
      address = address + self.DATA_SIZE
      written = written + self.DATA_SIZE
      if callback is not None:
        callback(min(written, length), length)

  def verify(self, start, length, data, callback = None):
    """ Verify data in the flash memory

        Verify the contents of the flash memory with the given bytes starting
        at address 'start' for 'length' bytes.

        @param start the start address to read from. Must be a positive integer
                     in the allowable address range for the device.
        @param length the number of bytes to read. Must be a positive integer
                      greater than zero such that start + length is in the valid
                      address range for the device.
        @param data an array of bytes to be verified against. The length of
                    the must be at least 'length' bytes.
        @param callback optional callback function used to report progress. It
                        will be passed two integer values - the number of bytes
                        that have been transferred and the number of bytes
                        requested.

        @throws MicrobootException if the data does not match, the address is
                out of range or a communication error occurs.
    """
    # Read the data
    current = self.read(start, length, callback = callback)
    # Do the verification
    for index in range(length):
      if data[index] <> current[index]:
        raise MicrobootException("Verification failed. Value at %04X is 0x%02X, expected 0x%02X." % (start + index, current[index], data[index]))
    # At this point it all worked.

  def reset(self):
    """ Reset the device.

        Asks the device to reset (start the application program).
    """
    pass

# Make sure we are just being imported
if __name__ == "__main__":
  print "Error: This file is a support library, do not run it directly."
  exit(1)
