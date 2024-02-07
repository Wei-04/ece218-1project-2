/*! @mainpage Automatic Headlight Control System
* @date Monday, January 24, 2024
* @authors Louis Rahilly, Weiheng He
* @section genDesc General Description
*
* This is an implementation of a headlight control system to be used in a car.
* The driver will have the ability to select on, off, or automatic headlights.
*
* @section changelog Changelog
* | Date | Description |
* |:----------:|:-----------------------------------------------|
* | 01/24/2024 | First version of program |
* | 01/25/2024 | Added framework for initialization and logic |
* | 01/28/2024 | Built engineUpdate |
* | 01/30/2024 | Built headLight function |
* | 02/1/2024 | Finished adding delays |
* 
*
*/

//=====[Libraries]=============================================================

#include "mbed.h" //library imports
#include "arm_book_lib.h"

//=====[Defines]===============================================================

#define POT_ON                                0.33 //less than 0.33 means headlights on
#define POT_OFF                               0.66 //between 0.33 and 0.66 meansheadlights off
#define DAY                                   0.7 //analog input threshold which determines daylight
#define DAYLIGHT_DELAY                        2000 //delay for tuning off
#define DUSK_DELAY                            1000 //delay for turning on
#define TIME_DELAY                            10 //time interval

//=====[Declaration and initialization of public global objects]===============

DigitalIn driverOccupancy(D2); // Object associated driver seat contact
DigitalIn ignitionButton(BUTTON1); // Object associated with attempted ignition
DigitalOut ignitionLED(LED2); // Object associated with sucessful igntion
DigitalOut headlightLED1(PE_14); //Object associated with headlights
DigitalOut headlightLED2(PE_15);

AnalogIn potentiometer(A0); //Potentiometer reading, used in selection of headlight mode 
AnalogIn lightLevel(A2); //Light sensitive resistor reading, used to determine presence of daylight

//=====[Declaration and initialization of public global variables]=============

float potentiometerR = 0.0; //keeps track of potentiometer reading
int accumulatedTime = 0; //keeps track of time interval

//=====[Declarations (prototypes) of public functions]=========================

void inputsInit(); /*Initalizes inputs:
driverOccupancy = OFF
*/

void outputsInit(); /*Initalizes outputs:
ignitionLED = OFF
headlightLED = OFF
*/

bool engineUpdate(); //controls the engine state, returns the current engine state

void headlightsUpdate(); //controls the headlights state based on the headlight mode

void potentiometerOutput(); //reads potentiometer output

UnbufferedSerial uartUsb(USBTX, USBRX, 115200); // Object associated with the uart process

//=====[Main function, the program entry point after power on or reset]========

int main() //main function loop
{
    inputsInit(); //initializations
    outputsInit();
    while (true) {
        engineUpdate(); //updating the engine states
        potentiometerOutput(); //updating the potentiometer states
        headlightsUpdate(); //updating the headlight states
        delay(TIME_DELAY); //our delay interval
        }
}


//=====[Implementation of global functions]====================================

void inputsInit() { //initializing inputs
    driverOccupancy.mode(PullDown);
}

void outputsInit() { //initializing outputs
    ignitionLED = OFF;
    headlightLED1 = OFF;
    headlightLED2 = OFF;
}

bool engineUpdate() { //updates the engine states
    if (ignitionButton) {
        if (driverOccupancy) {
            ignitionLED = ON;
            return true; //we return true if the engine is on
        }
        else {
            ignitionLED = OFF;
            return false; //we return false if engine is off
        }
    }
    if (ignitionLED == ON) { //returning after ignition button is let go of
        return true;
    }
    else {
        return false;
    }
}

void headlightsUpdate() { //updates the headlights
    potentiometerR = potentiometer.read(); //assigns the potentiometer
    if (engineUpdate()) { //checks to see if engine is on or not
        
        if (potentiometerR <= POT_ON) { //on headlight state
            headlightLED1 = ON;
            headlightLED2 = ON;
        }
        else if (potentiometerR < POT_OFF && potentiometerR > POT_ON) { //off headlight state, between on and auto
            headlightLED1 = OFF;
            headlightLED2 = OFF;
        }
        else if (potentiometerR >= POT_OFF) { //auto headlight state
            if (lightLevel > DAY) { //daytime arguement
                accumulatedTime = accumulatedTime + TIME_DELAY; //measure delay to turn off
                if (accumulatedTime >= DAYLIGHT_DELAY) { //resets delay and turns off
                    headlightLED1 = OFF;
                    headlightLED2 = OFF;         
                    accumulatedTime = 0;   
                }   
            }
            else { //nighttime arguement
                accumulatedTime = accumulatedTime + TIME_DELAY; //measure delay to turn on
                if (accumulatedTime >= DUSK_DELAY){ //resets delay and turns on
                    headlightLED1 = ON;
                    headlightLED2 = ON;
                    accumulatedTime >= 0;
                }
            }
            
        }
    }
    else { //turn off the headlights if the engine is off
        headlightLED1 = OFF;
        headlightLED2 = OFF;    
    }
}

void potentiometerOutput() { //reads and prints out the potentiometer
    int stringLength;
    char str[100];
    potentiometerR = potentiometer.read();
    sprintf (str, "Potentiometer: %.2f\r\n", potentiometerR);
    stringLength = strlen(str);
    uartUsb.write(str, stringLength);
}