/*
  nixie_write_left - sample code using the Nixie library 
                      for controlling the Ogi Lumen nixie tube driver.
  Created by Lionel Haims, July 25, 2008.
  Released into the public domain.
*/

// include the library
#include <Nixie.h>

// note the digital pins of the arduino that are connected to the nixie driver
#define dataPin 2  // data line or SER
#define clockPin 3 // clock pin or SCK
#define latchPin 4 // latch pin or RCK

// note the number of digits (nixie tubes) you have (buy more, you need more)
#define numDigits 4

// just some defines to simplify the examples
#define shortDelay 300
#define longDelay 5000

// Create the Nixie object
// pass in the pin numbers in the correct order
Nixie nixie(dataPin, clockPin, latchPin);

void setup()
{
  // Clear the display if you wish
  nixie.clear(numDigits);
}

void loop()
{
  // just some local variables to help the example
  int i = 0;

  // --- Example 1 ---
  // Write out the digits using a simple shift.
  // The number will be left justified, so if the number doesn't
  // use all the tubes, the right hand tubes will display whatever
  // was shifted into them.
  for(i=0; i <= 9; i++)
  {
    // Write the number
    nixie.writeNumLeft(i);
    delay(shortDelay);
  }
  
  // wait for the next example
  delay(longDelay);
}
