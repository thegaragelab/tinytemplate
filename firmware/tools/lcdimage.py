#!/usr/bin/env python
#----------------------------------------------------------------------------
# 28-Apr-2014 ShaneG
#
# This tool is used to generate data arrays for images to use with a Nokia
# LCD display.
#----------------------------------------------------------------------------
from sys import argv
from os.path import exists, basename, split, splitext

try:
  from PIL import Image, ImageDraw, ImageFont
except:
  print "ERROR: This utility requires the Python Imaging Library."
  exit(1)

#----------------------------------------------------------------------------
# Image processing
#----------------------------------------------------------------------------

MAX_IMAGE_WIDTH  = 64
MAX_IMAGE_HEIGHT = 32

""" Process an RGB file
"""
def imageToBits_RGB(image, oncolor):
  width, height = image.size
  width = min(width, MAX_IMAGE_WIDTH)
  height = min(height, MAX_IMAGE_HEIGHT)
  bits = ""
  for y in range(height):
    line = ""
    for x in range(width):
      color = image.getpixel((x, y))
      if (color[0] == oncolor[0]) and (color[1] == oncolor[1]) and (color[2] == oncolor[2]):
        line = line + "1"
      else:
        line = line + "0"
    bits = bits + line
  # All done
  return width, height, bits

""" Process an RGBA file
"""
def imageToBits_RGBA(image):
  width, height = image.size
  width = min(width, MAX_IMAGE_WIDTH)
  height = min(height, MAX_IMAGE_HEIGHT)
  bits = ""
  for y in range(height):
    line = ""
    for x in range(width):
      color = image.getpixel((x, y))
      if color[3] == 0:
        line = line + "0"
      else:
        line = line + "1"
    bits = bits + line
  # All done
  return width, height, bits

""" Process a monochrome image file
"""
def imageToBits_Mono(image, oncolor):
  width, height = image.size
  width = min(width, MAX_IMAGE_WIDTH)
  height = min(height, MAX_IMAGE_HEIGHT)
  bits = ""
  for y in range(height):
    line = ""
    for x in range(width):
      color = image.getpixel((x, y))
      if color == oncolor:
        line = line + "1"
      else:
        line = line + "0"
    bits = bits + line
  # All done
  return width, height, bits

""" Process a single image file
"""
def imageToBits(image, oncolor = 255):
  # Check the mode and process appropriately
  if image.mode == "RGB":
    if len(oncolor) == 1:
      oncolor = ( oncolor, oncolor, oncolor )
    return imageToBits_RGB(image, oncolor)
  if image.mode == "RGBA":
    return imageToBits_RGBA(image)
  if image.mode == "1":
    return imageToBits_Mono(image, oncolor)
  # If we get here, we failed
  print "  ERROR: Unsupported image type '%s'." % image.mode
  exit(1)

""" Process a single image file
"""
def processImage(filename, oncolor):
  # Make sure the file exists
  if not exists(filename):
    print "ERROR: The file '%s' does not exist." % filename
    exit(1)
  # Open it as an image
  image = None
  try:
    image = Image.open(filename)
  except:
    print "ERROR: Could not open '%s' as an image." % filename
    exit(1)
  # Start processing the file
  return imageToBits(image, oncolor)

""" Create a C file from the bits in the image
"""
def createCode(name, width, height, bits):
  # Pad the image to a height that is a multiple of 8
  padding = "0" * width
  top = True
  while (height % 8) <> 0:
    if top:
      bits = padding + bits
    else:
      bits = bits + padding
    height = height + 1
    top = not top
  # Now generate the values for each byte starting with the size
  results = [ ((int(height / 8) - 1) << 6) | (width - 1) ]
  for y in range(height / 8):
    for x in range(width):
      value = 0x00
      for bit in range(8):
        value = value << 1
        if bits[((((y + 1) * 8) - bit - 1) * width) + x] == '1':
          value = value | 0x01
      results.append(value)
  # Generate the output
  output = "const uint8_t IMAGE_%s[] PROGMEM = {\n" % name
  results = [ "0x%02x" % x for x in results ]
  while len(results) > 0:
    output = output + "  " + ", ".join(results[:12]) + ",\n"
    results = results[12:]
  output = output + "  };\n"
  return output

#----------------------------------------------------------------------------
# Main program
#----------------------------------------------------------------------------

USAGE = """
Usage:

%s [options] input-files

Description:

    This utility will generate C statements from a set of image files. The
    input files may be in a wide range of formats (jpg, png, bmp, etc) and
    the generated statements will have the same name as the input with the
    extension changed to '.c'.

Options:

    --invert

      This option only applies to monochrome images. It inverts the the meaning
      of the foreground color. Without the option any white pixels are set as
      the foreground, with the option applied black pixels are considered the
      foreground.
"""

if __name__ == "__main__":
  # Have we been given command line arguments ?
  if len(argv) <= 1:
    print USAGE % argv[0]
    exit(1)
  # Collect options
  oncolor = 255
  index = 1
  while argv[index].startswith("--"):
    if argv[index] == "--invert":
      oncolor = 0
      index = index + 1
    else:
      print "ERROR: Unrecognised option '%s'" % argv[index]
      exit(1)
  # Process the input files
  for filename in argv[index:]:
    width, height, bits = processImage(filename, oncolor)
    symbol = splitext(split(filename)[1])[0].upper()
    print createCode(symbol, width, height, bits)

