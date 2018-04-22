/**
 * Arduinix 6 Bulb ( IN-17 ) 
 * - Also supports Hour and Min. time set.
 *
 * This code runs a six bulb setup and displays a prototype clock setup.
 * NOTE: the delay is setup for IN-17 nixie bulbs.
 * by Jeremy Howa
 * www.robotpirate.com
 * www.arduinix.com
 * 2008 - 2009
 * Last Edit Aug 29, 2009
 *
 * NOTE: Major changes were basically formatting and pin selection based on my
 *       own PCB for the nixie tubes (NH-12A). Code contains RTC (DS1302)
 *       funtionalities too (https://playground.arduino.cc/Main/DS1302RTC).
 *       Complete project can be found in https://github.com/matiRLC/NixieClock
 * 
 * @author Matias Quintana matiasquitana.com
 * @version 2018 04 22
 */

#include <DS1302RTC.h>
#include <Time.h>

// Debug mode for sending the current time via serial 
#define DEBUG_ON true
#define SET_TIME true

/** Pin mapping for Arduino UNO, Atmega328P
 * ARDUINO UNO    Atmega328P  Used by
 *     A0            PC00     RTC
 *     A1            PC01     RTC
 *     A2            PC02     RTC
 *     PIN2          PD02     Nixie Driver
 *     PIN3          PD03     Nixie Driver
 *     PIN4          PD04     Nixie Driver
 *     PIN5          PD05     Nixie Driver
 *     PIN6          PD06     Nixie Driver
 *     PIN7          PD07     Nixie Driver
 *     PIN8          PB00     Nixie Driver
 *     PIN9          PB01     Nixie Driver
 *    PIN10          PB02     Nixie Driver
 *    PIN11          PB03     Nixie Driver
 *    PIN12          PB04     Nixie Driver
 */

// DEFINE
long MINS  = 60;         // 60 Seconds in a Min.
long HOURS = 60 * MINS;  // 60 Mins in an hour.
long DAYS  = 12 * HOURS; // 24 Hours in a day. > Note: change the 24 to a 12 for non millitary time.

long runTime = 0;        // Time from when we started.

// default time sets. clock will start at the indicated values below clockHourSet:clockMinSet:00
// NOTE: Seconds start at 0
long clockHourSet;// = 11;
long clockMinSet;//  = 45;

int HourButtonPressed = false;
int MinButtonPressed = false;

/**
 * SN74141: True Table
 * D  C  B  A  #     
 * L, L, L, L  0
 * L, L, L, H  1
 * L, L, H, L  2
 * L, L, H, H  3
 * L, H, L, L  4
 * L, H, L, H  5
 * L, H, H, L  6
 * L, H, H, H  7
 * H, L, L, L  8
 * H, L, L, H  9
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

// DS1302 Set pins:  CE, IO, CLK (RST, DAT, CLK)
DS1302RTC RTC(A2, A1, A0);

/**
 * setup()
 * Purpose: IO initialization and debugging flag set
 */
void setup() {
    if (DEBUG_ON) {
        Serial.begin(9600);
    }
    
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
 
    // NOTE:
    // Grounding on pints 14 and 15 will set the Hour and Mins.
    pinMode(14, INPUT);   // set the virtual pin 14 (pin 0 on the analog inputs ) 
    digitalWrite(14, HIGH); // set pin 14 as a pull up resistor.

    pinMode(15, INPUT);   // set the vertual pin 15 (pin 1 on the analog inputs ) 
    digitalWrite(15, HIGH); // set pin 15 as a pull up resistor.
  
    // Activate RTC module
    Serial.print("RTC activated\n");
    delay(500);

    // Check clock oscillation  
    if (RTC.haltRTC()) {
        Serial.print("Clock stopped!\n");
    } else {
        Serial.print("Clock working.\n");
    }

    // Check write-protection
    if (RTC.writeEN())
        Serial.print("Write allowed.\n");
    else
        Serial.print("Write protected.\n");

    delay(2000);

    // Setup Time library  
    Serial.print("RTC Sync\n");
    setSyncProvider(RTC.get); // the function to get the time from the RTC
    if(timeStatus() == timeSet) {
        Serial.print("Ok!\n");
        if(SET_TIME) { // once the time is sent, change boolean flag so it remains saved
            setTime(0,59,12,22,04,2018); // change time accordingly
            RTC.set(now());
        }
    } else {
        Serial.print("FAIL!\n");
    }
    delay(2000);

    // initiate time based on RTC readings
    tmElements_t tm;
    RTC.read(tm);
    clockMinSet = tm.Minute; 
    clockHourSet = tm.Hour;
}

/**
 * DisplayNumberSet
 * Purpose: Looks up the truth table and opens the correct outs from the arduino
 * to light the numbers given to this funciton (num1,num2), on a 6 nixie bulb setup. 
 * @var anod anode number
 * @var num1 bulb 1 value
 * @var num2 bulb 2 value
 **/
void DisplayNumberSet (int anod, int num1, int num2) {
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
    }
  
    // Load the a,b,c,d.. to send to the SN74141 IC (1)
    switch (num1) {
        case 0: a = 0; b = 0; c = 0; d = 0; break;
        case 1: a = 1; b = 0; c = 0; d = 0; break;
        case 2: a = 0; b = 1; c = 0; d = 0; break;
        case 3: a = 1; b = 1; c = 0; d = 0; break;
        case 4: a = 0; b = 0; c = 1; d = 0; break;
        case 5: a = 1; b = 0; c = 1; d = 0; break;
        case 6: a = 0; b = 1; c = 1; d = 0; break;
        case 7: a = 1; b = 1; c = 1; d = 0; break;
        case 8: a = 0; b = 0; c = 0; d = 1; break;
        case 9: a = 1; b = 0; c = 0; d = 1; break;
    }  
  
    // Write to output pins.
    digitalWrite(ledPin_0_d, d);
    digitalWrite(ledPin_0_c, c);
    digitalWrite(ledPin_0_b, b);
    digitalWrite(ledPin_0_a, a);

    // Load the a,b,c,d.. to send to the SN74141 IC (2)
    switch (num2) {
        case 0: a = 0; b = 0; c = 0; d = 0; break;
        case 1: a = 1; b = 0; c = 0; d = 0; break;
        case 2: a = 0; b = 1; c = 0; d = 0; break;
        case 3: a = 1; b = 1; c = 0; d = 0; break;
        case 4: a = 0; b = 0; c = 1; d = 0; break;
        case 5: a = 1; b = 0; c = 1; d = 0; break;
        case 6: a = 0; b = 1; c = 1; d = 0; break;
        case 7: a = 1; b = 1; c = 1; d = 0; break;
        case 8: a = 0; b = 0; c = 0; d = 1; break;
        case 9: a = 1; b = 0; c = 0; d = 1; break;
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
    // bank 1 (bulb 0, 3)
    DisplayNumberSet(0, array[0], array[3]);   
    // bank 2 (bulb 1, 4)
    DisplayNumberSet(1, array[4], array[1]); //it should be (1, array[1], array[4]) but there was a mistake in the first PCB
    // bank 3 (bulb 2, 5)
    DisplayNumberSet(2, array[2], array[5]);   
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
      
    if (DEBUG_ON) {
        //Serial.println(hourInput);
        tmElements_t tm;
        RTC.read(tm);
        Serial.print(tm.Minute, DEC);
        Serial.print("\n");
    }
    
    if (hourInput == 0)
        HourButtonPressed = true;

    if (minInput == 0)
        MinButtonPressed = true;
  
    if (HourButtonPressed == true && hourInput == 1) {
        clockHourSet++;
        HourButtonPressed = false;
    }
  
    if (MinButtonPressed == true && minInput == 1) {
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
    int NumberArray[6] = {0, 0, 0, 0, 0, 0};
    NumberArray[0] = upperHours;
    NumberArray[1] = lowerHours;
    NumberArray[2] = upperMins;
    NumberArray[3] = lowerMins;
    NumberArray[4] = upperSeconds;
    NumberArray[5] = lowerSeconds;

    // Display.
    DisplayNumberString(NumberArray);
}

