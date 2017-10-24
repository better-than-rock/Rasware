#include <RASLib/inc/common.h> 
#include <RASLib/inc/gpio.h> 
#include <RASLib/inc/time.h> 
#include <RASLib/inc/adc.h>
#include <RASLib/inc/sonar.h> 
#include <RASLib/inc/motor.h>
#include "Switch.h"

// Blink the LED to show we're on
tBoolean blink_on = true;


void blink(void) {
    SetPin(PIN_F3, blink_on);
    blink_on = !blink_on;
}


// The 'main' function is the entry point of the program
int main(void) {
    tMotor *left = InitializeServoMotor(PIN_B0, true);
    tMotor *right = InitializeServoMotor(PIN_B7, false);
    tADC *disLeft = InitializeADC(PIN_B5);
    float leftInput;
    tADC *disRight = InitializeADC(PIN_B2);
    float rightInput;
    //These are arbitrary values to be tested and changed. 
    float maxDist = 50;
    float minDist = 10;
    float kP = -0.01;
    float errorL = ADCRead(disLeft) - maxDist;
    float errorR = ADCRead(disRight) - maxDist;

    Printf("Hello World!\n");
    // Initialization code can go here
    CallEvery(blink, 0, 0.5);
    while (1) {
        // Runtime code can go here
        
        //SetMotor(left, 1.0);
        //SetMotor(right, 1.0);
        rightInput = ADCRead(disRight);
	leftInput  = ADCRead(disLeft);
	Printf("Right: %f", rightInput);
        Printf("Left: %f", leftInput);
	//This is for testing, actual speeds should be calculated based on input from sensors.
	if((rightInput > 100)) {
	    SetMotor(left, 1.0);
	    SetMotor(right, 0.5);
	} else if((leftInput > 100)) { 
	    SetMotor(right, 1.0);
	    SetMotor(left, 0.5);
	}
    }
}

