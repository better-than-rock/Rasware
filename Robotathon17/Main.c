#include <RASLib/inc/common.h> 
#include <RASLib/inc/gpio.h> 
#include <RASLib/inc/time.h> 
#include "Switch.h"
#include <RASLib/inc/sonar.h> 

// Blink the LED to show we're on
tBoolean blink_on = true;
tMotor *left = InitializeServoMotor(PIN_B0, true);
tMotor *right = InitializeServoMotor(PIN_B7, false);
tSonar *DisLeft = InitializeSonar(<PIN>, <PIN>);
tSonar *DisRight = InitializeSonar(<PIN>, <PIN>);

void blink(void) {
    SetPin(PIN_F3, blink_on);
    blink_on = !blink_on;
}


// The 'main' function is the entry point of the program
int main(void) {

    //These are arbitrary values to be tested and changed. 
    float maxDist = 50;
    float minDist = 10;
    float kP = 3.14;
    float errorL = ADCRead(DisLeft) - maxDist;
    float errorR = ADCRead(DisRight) - maxDist;

    // Initialization code can go here
    CallEvery(blink, 0, 0.5);
    while (1) {
        // Runtime code can go here
        Printf("Hello World!\n");
        //SetMotor(left, 1.0);
        //SetMotor(right, 1.0);
        
	//This is for testing, actual speeds should be calculated based on input from sensors.
	if(ADCRead(DisRight > 100)) {
	    SetMotor(left, 1.0);
	    SetMotor(right, 0.5);
	} else if(ADCRead(DisLeft > 100)) { 
	    SetMotor(right, 1.0);
	    SetMotor(left, 0.5);
	}
    }
}

