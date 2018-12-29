// fading transitions sketch for 6-tube board with default connections.
// based on 6-tube sketch by Emblazed

// 09/03/2010 - Added Poxin's 12 hour setting for removing 00 from hours when set to 12 hour time
// 06/16/2011 - 4-tube-itized by Dave B.
// 08/19/2011 - modded for six bulb board, hours, minutes, seconds by Brad L.
// 01/28/2013 - expanded to 8 digit crossfade by Brad L.
// 12/04/2014 - First V2 standardization version by Brad L. 
// 01/07/2015 - Code Revision by Jeremy Howa
// 05/17/2015 - Added light sensor support to auto dim tubes.  Added neopixel driver - Brad L. 
// 12/30/2016 - commented out neopixel code in case user hasn't included that library in local files. Set as standard 6xIN17, 4xINS1 codebase

// 19/08/2018 - adapted original code for a more stripped down version of just 6 bulbs + colons, matiasquintana.com

// DEFINE
#define _DEBUG          0
long MINS  = 60;         // 60 Seconds in a Min.
long HOURS = 60 * MINS;  // 60 Mins in an hour.
long DAYS  = 24 * HOURS; // 24 Hours in a day. > Note: change the 24 to a 12 for non military time.

long runTime = 0;       // Time from when we started.

// default time sets. clock will start at 12:34:56.  This is so we can count the correct order of tubes.
long clockHourSet = 14;
long clockMinSet  = 25;
long clockSecSet  = 00;

int HourButtonPressed = false;
int MinButtonPressed = false;

/**
 * SN74141: True Table
 * D  C  B  A  #     
 * L, L, L, L  9
 * L, L, L, H  8
 * L, L, H, L  7
 * L, L, H, H  6
 * L, H, L, L  5
 * L, H, L, H  4
 * L, H, H, L  3
 * L, H, H, H  2
 * H, L, L, L  1
 * H, L, L, H  0
 */

// SN74141 (1)
int ledPin_0_a = 2;                
int ledPin_0_b = 3;
int ledPin_0_c = 4;
int ledPin_0_d = 5;

// SN74141 (2)
int ledPin_1_a = 6;                
int ledPin_1_b = 7;
int ledPin_1_c = 8;
int ledPin_1_d = 9;

// anode pins
int ledPin_a_1 = 10;
int ledPin_a_2 = 11;
int ledPin_a_3 = 12;
int ledPin_a_4 = 13;

/**
 * setup()
 * Purpose: IO initialization and debugging flag set
 */
void setup() {
    pinMode(ledPin_0_a, OUTPUT);      
    pinMode(ledPin_0_b, OUTPUT);      
    pinMode(ledPin_0_c, OUTPUT);      
    pinMode(ledPin_0_d, OUTPUT);    
  
    pinMode(ledPin_1_a, OUTPUT);      
    pinMode(ledPin_1_b, OUTPUT);      
    pinMode(ledPin_1_c, OUTPUT);      
    pinMode(ledPin_1_d, OUTPUT);      
  
    pinMode(ledPin_a_1, OUTPUT);      
    pinMode(ledPin_a_2, OUTPUT);      
    pinMode(ledPin_a_3, OUTPUT);   
    pinMode(ledPin_a_4, OUTPUT);    
 
    // NOTE: Grounding on pints 14 and 15 will set the Hour and Mins.
    pinMode(14, INPUT); // set the virtual pin 14 (pin 0 on the analog inputs )  SET HOURS up
    digitalWrite(14, HIGH); // set pin 14 as a pull up resistor.

    pinMode(15, INPUT); // set the virtual pin 15 (pin 1 on the analog inputs )  SET MINUTES up
    digitalWrite(15, HIGH); // set pin 15 as a pull up resistor.
 
    if( _DEBUG ) {
        Serial.begin(9600); 
    }
}

/**
 * DisplayNumberSet
 * Purpose: Looks up the truth table and opens the correct outs from the arduino
 * to light the numbers given to this funciton (num1,num2), on a 6 nixie bulb setup. 
 * @var anod anode number
 * @var num1 bulb 1 valueclockMinSet
 * @var num2 bulb 2 value
 **/
void DisplayNumberSet(int anod, int num2, int num1) {
    int anodPin;
    int a,b,c,d;  
    
    // set defaults.
    a = 0; b = 0; c = 0; d = 0; // will display a zero.
    anodPin = ledPin_a_1;       // default on first anod.

    // Select what anod to fire.
    switch (anod) {
        case 0: anodPin = ledPin_a_1; break;
        case 1: anodPin = ledPin_a_2; break;
        case 2: anodPin = ledPin_a_3; break;
        case 3: anodPin = ledPin_a_4; break;
    }
    
    // Load the a,b,c,d.. to send to the SN74141 IC (1)
    switch (num1) {
        case 9: a = 0; b = 0; c = 0; d = 0; break;
        case 8: a = 1; b = 0; c = 0; d = 0; break;
        case 7: a = 0; b = 1; c = 0; d = 0; break;
        case 6: a = 1; b = 1; c = 0; d = 0; break;
        case 5: a = 0; b = 0; c = 1; d = 0; break;
        case 4: a = 1; b = 0; c = 1; d = 0; break;
        case 3: a = 0; b = 1; c = 1; d = 0; break;
        case 2: a = 1; b = 1; c = 1; d = 0; break;
        case 1: a = 0; b = 0; c = 0; d = 1; break;
        case 0: a = 1; b = 0; c = 0; d = 1; break;
    }  
  
    // Write to output pins.
    digitalWrite(ledPin_0_d, d);
    digitalWrite(ledPin_0_c, c);
    digitalWrite(ledPin_0_b, b);
    digitalWrite(ledPin_0_a, a);

    // Load the a,b,c,d.. to send to the SN74141 IC (2)
    switch (num2) {
        case 9: a = 0; b = 0; c = 0; d = 0; break;
        case 8: a = 1; b = 0; c = 0; d = 0; break;
        case 7: a = 0; b = 1; c = 0; d = 0; break;
        case 6: a = 1; b = 1; c = 0; d = 0; break;
        case 5: a = 0; b = 0; c = 1; d = 0; break;
        case 4: a = 1; b = 0; c = 1; d = 0; break;
        case 3: a = 0; b = 1; c = 1; d = 0; break;
        case 2: a = 1; b = 1; c = 1; d = 0; break;
        case 1: a = 0; b = 0; c = 0; d = 1; break;
        case 0: a = 1; b = 0; c = 0; d = 1; break;
    }
  
    // Write to output pins
    digitalWrite(ledPin_1_d, d);
    digitalWrite(ledPin_1_c, c);
    digitalWrite(ledPin_1_b, b);
    digitalWrite(ledPin_1_a, a);
    
    // Turn on this anod.
    digitalWrite(anodPin, HIGH);   

    // Delay
    // NOTE: With the differnce in Nixie bulbs you may have to change
    //       this delay to set the update speed of the bulbs. If you 
    //       dont wait long enough the bulb will be dim or not light at all
    //       you want to set this delay just right so that you have 
    //       nice bright output yet quick enough so that you can multiplex with
    //       more bulbs.
    delay(2);
  
    // Shut off this anod.
    digitalWrite(anodPin, LOW);
}

/**
 * DisplayNumberString
 * Purpose: passing an array that is 8 elements long will display numbers
 *          on a 6 nixie bulb setup.
 * @var array int* array containing the values for all 6 nixie tubes
 */
void DisplayNumberString (int* array) {
    
    // colons
    DisplayNumberSet(0, array[0], array[4]);
    // bank 1 (bulb 4, 8)
    DisplayNumberSet(1, array[3], array[7]);
    // bank 2 (bulb 2, 7)
    DisplayNumberSet(2, array[2], array[6]);
    // bank 3 (bulb 1, 5)
    DisplayNumberSet(3, array[1], array[5]);
}

/**
 * Loop
 * Purpose: Main function that will continue to be executed as long as the system is energised
 */
void loop() {
    // Get milliseconds.
    runTime = millis();
    
    // Get time in seconds.
    long time = (runTime) / 1000;
    
    int hourInput = digitalRead(14);  
    int minInput  = digitalRead(15);

    if(hourInput == 0)
        HourButtonPressed = true;
    
    if(minInput == 0)
        MinButtonPressed = true;
    
    if(HourButtonPressed == true && hourInput == 1) {
        clockHourSet++;
        HourButtonPressed = false;
    }
    
    if(MinButtonPressed == true && minInput == 1) {
        clockMinSet++;
        MinButtonPressed = false;
    }
    
     // Set time based on offset..
    long hbump = 60*60*clockHourSet;
    long mbump = 60*clockMinSet;
    time += mbump + hbump;

    // Convert time to days,hours,mins,seconds
    long days  = time / DAYS;       time -= days  * DAYS; 
    long hours = time / HOURS;      time -= hours * HOURS; 
    long minutes  = time / MINS;    time -= minutes  * MINS; 
    long seconds  = time; 
    
    // Get the high and low order values for hours,min,seconds. 
    int lowerHours = hours % 10;
    int upperHours = hours - lowerHours;
    int lowerMins = minutes % 10;
    int upperMins = minutes - lowerMins;
    int lowerSeconds = seconds % 10;
    int upperSeconds = seconds - lowerSeconds;
    if (upperSeconds >= 10) upperSeconds = upperSeconds / 10;
    if (upperMins >= 10) upperMins = upperMins / 10;
    if (upperHours >= 10) upperHours = upperHours / 10;
 
    // Fill in the Number array used to display on the tubes.
    int NumberArray[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    NumberArray[0] = 0; // colons
    NumberArray[1] = upperHours;
    NumberArray[2] = lowerHours;
    NumberArray[3] = upperMins;
    NumberArray[4] = 0; // colons
    NumberArray[5] = lowerMins;
    NumberArray[6] = upperSeconds;
    NumberArray[7] = lowerSeconds;
    
    // Display.
    DisplayNumberString(NumberArray);
}



