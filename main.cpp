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
* 
*
*/

//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

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

int engineUpdate(); //controls the engine state, returns the current engine state

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

int engineUpdate() {
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
        uartUsb.write("hi\r\n", 4);
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