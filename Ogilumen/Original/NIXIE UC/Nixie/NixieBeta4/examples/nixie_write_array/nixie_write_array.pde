/*
  nixie_write_array - sample code using the Nixie library 
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

  // --- Example 4 ---
  // Write out the array of digits.
  // The entire array will be displayed.
  // Note, a digit with a value of 10 will be blank.

  // create an array to hold the digits
  int arrayNums[numDigits];

  for(i=0; i < numDigits; i++)
  {
    // fill the array with something
    arrayNums[i] = random(10);
  }

  // Write out the array
  nixie.writeArray( arrayNums, numDigits);
    delay(longDelay);

  // NOTE: the first index [0] is the leftmost digit
  // Change the first (left) digit
  arrayNums[0] = random(10);
  // Write out the array
  nixie.writeArray( arrayNums, numDigits);
  delay(longDelay);
  
  // The highest index is the rightmost digit
  // Change the last (right) digit
  arrayNums[numDigits-1] = random(10);
  // Write out the array
  nixie.writeArray( arrayNums, numDigits);
  delay(longDelay);
}
