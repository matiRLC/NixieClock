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
 *       own PCB for the nixie tubes (NH-12A)
 * @author Matias Quintana matiasquitana.com
 * @version 16/01/17
 */

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

// Debug mode for sending the current time via serial 
#define DEBUG_ON true

/** Pin mapping for ADC, Arduino UNO, Atmega328P
 * ARDUINO UNO    Atmega328P
 *     PIN2          PD2
 *     PIN3          PD3
 *     PIN4          PD4
 *     PIN5          PD5
 *     PIN6          PD6
 *     PIN7          PD7
 *     PIN8          PB0
 *     PIN9          PB1
 *    PIN10          PB2
 *    PIN11          PB3
 *    PIN12          PB4
 *    PIN14          PC0
 *    PIN15          PC1
 */

/**
 * setup()
 * Purpose: IO initialization and debugging flag set
 */
void setup() {
    // SN74141 (1)
    // Output: PD2:PD5
    // PD2 = a, PD3 = b, PD4 = c, PD5 = d
    DDRD |= 0b11111100;
    // SN74141 (2)
    // Output: PD6:PD7 and PB0:PB1
    // PD6 = a, PD7 = b, PB0 = c, PB1 = d
    DDRB |= 0b00011111;
    // Anode pins:PB2:PB4
    // PB2: anode1
    // PB3: anode2
    // PB4: anode3

    // The following pins can be wired to buttons
    // Grounding on pins PC0 and PC1 will set the Hour and Mins.
    // Input: PC0:PC1
    DDRC |= 0b00000000;
    // Set pull up resistors in those pins
    PORTC |= 0b00000011;  
  
    if (DEBUG_ON) {
        Serial.begin(9600);
    }
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
    int a,b,c,d;

    // set defaults.
    a = 0; b = 0; c = 0; d = 0; // will display a zero.
  
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
    
    // Write the value in SN74141 (1)
    // set nth bit to value x
    // number = number & ~(1 << n) | (x << n);
    PORTD = PIND & ~(1 << PD5) | (d << PD5); // d
    PORTD = PIND & ~(1 << PD4) | (c << PD4); // c
    PORTD = PIND & ~(1 << PD3) | (b << PD3); // b
    PORTD = PIND & ~(1 << PD2) | (a << PD2); // a

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

    // Write the value in SN74141 (2)
    PORTB = PINB & ~(1 << PB1) | (d << PB1); // d
    PORTB = PINB & ~(1 << PB0) | (c << PB0); // c
    PORTD = PIND & ~(1 << PD7) | (b << PD7); // b
    PORTD = PIND & ~(1 << PD6) | (a << PD6); // a

    // Select what node to fire and turn on that node (HIGH)
    switch (anod) {
        case 0: PORTB |= 1 << PB2; break;
        case 1: PORTB |= 1 << PB3; break;
        case 2: PORTB |= 1 << PB4; break;
    }

    // Delay
    // NOTE: With the differnce in Nixie bulbs you may have to change
    //       this delay to set the update speed of the bulbs. If you 
    //       dont wait long enough the bulb will be dim or not light at all
    //       you want to set this delay just right so that you have 
    //       nice bright output yet quick enough so that you can multiplex with
    //       more bulbs.
    delay(2);
  
    switch (anod) { // Turn off this anode (LOW)
        case 0: PORTB &= ~(1 << PB2); break;
        case 1: PORTB &= ~(1 << PB3); break;
        case 2: PORTB &= ~(1 << PB4); break;
    }
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

// DEFINE
long MINS  = 60;         // 60 Seconds in a Min.
long HOURS = 60 * MINS;  // 60 Mins in an hour.
long DAYS  = 12 * HOURS; // 24 Hours in a day. > Note: change the 24 to a 12 for non millitary time.

long runTime = 0;        // Time from when we started.

// default time sets. clock will start at the indicated values below clockHourSet:clockMinSet:00
// NOTE: Seconds start at 0
long clockHourSet = 12;
long clockMinSet  = 34;

int HourButtonPressed = false;
int MinButtonPressed = false;

/**
 * Loop
 * Purpose: Main function that will continue to be executed as long as the system is energised
 */
void loop() {
    // Get milliseconds.
    runTime = millis();

    // Get time in seconds.
    long time = (runTime) / 1000;

    int hourInput = PINC & 0x01; // PC0
    int minInput = PINC & 0x02;  // PC1
      
    if(DEBUG_ON)
        Serial.println( hourInput );       
  
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
    long hbump = 60 * 60 * clockHourSet;
    long mbump = 60 * clockMinSet;
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

