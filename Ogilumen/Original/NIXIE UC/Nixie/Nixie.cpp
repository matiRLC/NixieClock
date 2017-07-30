/*
  Nixie.cpp - Library for controlling the Ogi Lumen nixie tube driver.
  Created by Lionel Haims, July 25, 2008.
  Released into the public domain.
  
  Rev 2: July 27, 2008
  Rev 3: March 1, 2009:
         Added include of WProgram.h to fix compile error in arduino IDE 12/13
*/

// Include the standard types
#include <WProgram.h>
#include <WConstants.h>
#include "Nixie.h"

#define _BLANK_NUM 10

int elementOrder[]={1,6,2,7,5,0,4,9,8,3};

// Constructor
// dataPin  -> nixie driver pin 1 (SER)
// clockPin -> nixie driver pin 3 (SCK)
// latchPin -> nixie driver pin 4 (RCK)
Nixie::Nixie(int dataPin, int clockPin, int latchPin)
{
  // save our member data
  _dataPin = dataPin;
  _clockPin = clockPin;
  _latchPin = latchPin;

  // set the pins to output mode
  pinMode(_dataPin, OUTPUT);
  pinMode(_clockPin, OUTPUT);
  pinMode(_latchPin, OUTPUT);

  // set the pins low
  digitalWrite(_dataPin, 0);
  digitalWrite(_clockPin, 0);
  digitalWrite(_latchPin, 0);
}

    
// Write a full number, left justified, pushing digits to the right
//   number = the number to write out
void Nixie::writeNumLeft(int number)
{
  // Set the latch pin low
  digitalWrite(_latchPin, 0);

  if(number == 0)
  {
    _serializeDigit(0);
  }
  
  // Write the digits out from right to left
  while(number > 0)
  {
    _serializeDigit(number%10);
    number = number / 10;
  }

  // Set the latch pin high
  digitalWrite(_latchPin, 1);

  // Set the latch pin low
  digitalWrite(_latchPin, 0);
}

// Write a full number, right justified, padded with zeros
//   number = the number to write out
//   digits = the number of digits (nixie tubes) 
void Nixie::writeNumZero(int number, int digits)
{
  int i = 0;
  
  // Set the latch pin low
  digitalWrite(_latchPin, 0);

  // Write the digits out from right to left
  for(i = 0; i < digits; i++)
  {
    _serializeDigit(number%10);
    number = number / 10;
  }

  // Set the latch pin high
  digitalWrite(_latchPin, 1);

  // Set the latch pin low
  digitalWrite(_latchPin, 0);
}

// Write a full number, right justified, padded with blanks
//   number = the number to write out
//   digits = the number of digits (nixie tubes) 
void Nixie::writeNumTrim(int number, int digits)
{
  int i = 0;
  
  // Set the latch pin low
  digitalWrite(_latchPin, 0);

  if(number == 0)
  {
    _serializeDigit(0);
    digits--;
  }

  // Write the digits out from right to left
  for(i = 0; i < digits; i++)
  {
    if(number == 0)
    {
      _serializeDigit(_BLANK_NUM);
    }
    else
    {
      _serializeDigit(number%10);
    }
    
    number = number / 10;
  }

  // Set the latch pin high
  digitalWrite(_latchPin, 1);

  // Set the latch pin low
  digitalWrite(_latchPin, 0);
}

// Write the digits of an array of specified size
//   arrayNums = the array of digits
//   size      = the number to digits
// note: a digit with a value of 10 will display a blank
void Nixie::writeArray(int* arrayNums, int size)
{
  int i = 0;
  
  // Set the latch pin low
  digitalWrite(_latchPin, 0);

  // Write the digits out from right to left
  // The low index is left, the high index is right
  for(i = size; i > 0; i--)
  {
    _serializeDigit(arrayNums[i-1]);
  }

  // Set the latch pin high
  digitalWrite(_latchPin, 1);

  // Set the latch pin low
  digitalWrite(_latchPin, 0);
}

// Clear the display
//   digits = the number of digits (nixie tubes)
void Nixie::clear(int digits)
{
  int i = 0;
  
  // Set the latch pin low
  digitalWrite(_latchPin, 0);

  // Write the digits out from right to left
  for(i = 0; i < digits; i++)
  {
    _serializeDigit(10);
  }

  // Set the latch pin high
  digitalWrite(_latchPin, 1);

  // Set the latch pin low
  digitalWrite(_latchPin, 0);
}

// Private function that sends a digit out to the shift register
// writeDigit must be 0 <= x <= 10
void Nixie::_serializeDigit(byte writeDigit)
{
  // local variables
  int i = 0;
  int bitmask = 8; // binary 1000

  // set data pin low
  digitalWrite(_dataPin, 0);

  // send out the bits of the nibble MSB -> LSB
  for (i = 0; i < 4; i++)
  {
    // set clock low
    digitalWrite(_clockPin, 0);

    // write bit to the data pin
    digitalWrite(_dataPin, (bitmask & writeDigit));
    
    // set the clock high
    digitalWrite(_clockPin, 1);
    
    // shift the bitmask
    bitmask = bitmask >> 1;
  }

  // set the data low
  digitalWrite(_dataPin, 0);

  // set the clock low
  digitalWrite(_clockPin, 0);
}


    
