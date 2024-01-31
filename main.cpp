/*! @mainpage Automatic Headlight Control System
* @date Monday, January 24, 2024
* @authors Louis Rahilly, Weiheng He
* @section genDesc General Description
*
* This is an implementation of a headlight control system to be used in a 1995 McLaren F1 LM.
* The driver will have the ability to select on, off, or automatic headlights.
*
* @section changelog Changelog
* | Date | Description |
* |:----------:|:-----------------------------------------------|
* | 01/24/2024 | First version of program |
* | 01/25/2024 | Added framework for initialization and logic |
* | 01/28/2024 | Built engineUpdate |
* | 01/30/2024 | Built headLight function |
* 
*
*/

//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

#define POT_ON                                0.33
#define POT_OFF                               0.66
#define DAY                                   0.5
#define DAYLIGHT_DELAY                        2000
#define DUSK_DELAY                            1000
#define TIME_DELAY                            10

//=====[Declaration and initialization of public global objects]===============

DigitalIn driverOccupancy(D2); // Object associated driver seat contact
DigitalIn ignitionButton(BUTTON1); // Object associated with attempted ignition
DigitalOut ignitionLED(LED2); // Object associated with sucessful igntion
DigitalOut headlightLED1(PE_14); //Object associated with headlights
DigitalOut headlightLED2(PE_15);

AnalogIn potentiometer(A0); //Potentiometer reading, used in selection of headlight mode 
AnalogIn lightLevel(A2); //Light sensitive resistor reading, used to determine presence of daylight

//=====[Declaration and initialization of public global variables]=============

float potentiometerR = 0.0;

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

void potentiometerOutput(); 

UnbufferedSerial uartUsb(USBTX, USBRX, 115200); // Object associated with the uart process

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    inputsInit();
    outputsInit();
    while (true) {
        engineUpdate();
        potentiometerOutput();
        delay(TIME_DELAY);
        }
}


//=====[Implementation of global functions]====================================

void inputsInit() {
    driverOccupancy.mode(PullDown);
}

void outputsInit() {
    ignitionLED = OFF;
    headlightLED1 = OFF;
    headlightLED2 = OFF;
}

bool engineUpdate() {
    if (ignitionButton) {
        if (driverOccupancy) {
            ignitionLED = ON;
            return true;
        }
        else {
            ignitionLED = OFF;
            return false;
        }
    }
    return false;
}

void headlightsUpdate() {
    if (engineUpdate()) {
        if (potentiometer <= POT_ON) {
            headlightLED1 = ON;
            headlightLED2 = ON;
        }
        else if (potentiometer <= POT_OFF) {
            headlightLED1 = OFF;
            headlightLED2 = OFF;
        }
        else {
            if (lightLevel > DAY) { // Daytime arguement
                headlightLED1 = OFF;
                headlightLED2 = OFF;               
            }
            else {
                headlightLED1 = ON;
                headlightLED2 = ON;
            }
            
        }
    }
}

void potentiometerOutput() {
    int stringLength;
    char str[100];
    potentiometerR = potentiometer.read();
    sprintf (str, "Potentiometer: %.2f\r\n", potentiometerR);
    stringLength = strlen(str);
    uartUsb.write(str, stringLength);
}