/*
  nixie_write_trim - sample code using the Nixie library 
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
#define shortDelay 100
#define longDelay 5000
#define maxCount 200

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
  int highestNum = pow(10,numDigits)-1;
  
  // limit the counter just to make sure we don't have to sit and watch it all day
  if(highestNum > maxCount)
  {
    highestNum = maxCount;
  }

  // --- Example 3 ---
  // Write out the number using a blank padding.
  // The number will be right justified. The tubes on the left 
  // will be off if the number doesn't use all tubes.
  for(i=0; i <= highestNum; i++)
  {
    // Write the number
    nixie.writeNumTrim( i, numDigits);
    delay(shortDelay);
  }
  
  // wait for the next example
  delay(longDelay);
}
