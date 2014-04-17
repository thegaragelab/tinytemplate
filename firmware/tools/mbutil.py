#!/usr/bin/env python
#----------------------------------------------------------------------------
# 17-Apr-2014 ShaneG
#
# Helper functions for the Microboot utilities.
#----------------------------------------------------------------------------
from sys import stdout

# Number of steps in a progress bar
PROGRESS_STEPS = 30

# Where we are currently at
g_steps = 0

def beginProgress(taskName = None):
  """ Set up the progress meter

    @param taskName the name of the task. If specified this will appear in
                    the progress indicator guide.
  """
  title = "+-"
  if taskName is not None:
    title = title + " " + taskName.strip() + " "
  # Fill out the rest of the title
  title = title + ("-" * (PROGRESS_STEPS - len(title) - 1)) + "+"
  print title
  # Initialise the number of steps
  global g_steps
  g_steps = 0

def updateProgress(completed, total):
  """ Update the progress bar

    @param completed the amount of work currently completed
    @param total the total amount of work requested
  """
  if completed == 0:
    return # No progress yet
  # Calculate what to show
  global g_steps
  target = int((completed * PROGRESS_STEPS) / total)
  while g_steps < target:
    stdout.write('#')
    g_steps = g_steps + 1
  stdout.flush()

def endProgress():
  """ Clean up the progress meter
  """
  global g_steps
  while g_steps < PROGRESS_STEPS:
    stdout.write('#')
    g_steps = g_steps + 1
  stdout.write('\n')
  stdout.flush()

# Testing
if __name__ == "__main__":
  # First test, no title
  beginProgress()
  for x in range(1000):
    updateProgress(x, 1000 - 1)
  endProgress()
  # Test with a title
  beginProgress("Uploading")
  for x in range(PROGRESS_STEPS * 100):
    if (x % 150) == 0:
      updateProgress(x, (PROGRESS_STEPS * 100) - 1)
  endProgress()
