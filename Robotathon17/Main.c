#include <RASLib/inc/common.h> 
#include <RASLib/inc/gpio.h> 
#include <RASLib/inc/time.h> 
#include <RASLib/inc/adc.h>
#include <RASLib/inc/sonar.h> 
#include <RASLib/inc/motor.h>
#include <math.h>
#include "Switch.h"

// Blink the LED to show we're on
tBoolean blink_on = true;


void blink(void) {
    SetPin(PIN_F3, blink_on);
    blink_on = !blink_on;
}

void SetSpeed(tMotor* motor, float power) {
    float nErr = 0.38;
    SetMotor(motor, power * nErr);
}


// The 'main' function is the entry point of the program
int main(void) {
    tMotor *left = InitializeServoMotor(PIN_B0, false);
    tMotor *right = InitializeServoMotor(PIN_B7, true);
    tADC *disLeft = InitializeADC(PIN_E3);
    tADC *disRight = InitializeADC(PIN_E2);
    float sweetSpot = 0.6;
    float avgTriDelta = ADCRead(disLeft);
    float tempTriDelt = 0.0;
    int countTriDeltAvg = 0;
    //These are arbitrary values to be tested and changed. 
    // float maxDist = 50;
    // float minDist = 10;
    // float kP = -0.01;
    // float errorL = ADCRead(disLeft) - maxDist;
    // float errorR = ADCRead(disRight) - maxDist;
    // tBoolean lastPressed = false;

    Printf("Hello World!\n");
    // Initialization code can go here
    CallEvery(blink, 0, 0.5);
    // Runtime code can go here
    Printf("Hello World!\n");
    while (1) {
        float travelDirection = 0.0; // -1 => left +1 => right
        rightDistance = ADCRead(disRight);
        leftDistance  = ADCRead(disLeft);)
        if (avgTriDelta - leftDistance > .3) { // Drastic change in distance => we need to turn to keep up with wall
            travelDirection = -avgTriDelta;
        } else if (fabs(avgTriDelta - sweetSpot) > .2) { // Try to get in the .6 sweet spot
            travelDirection = (avgTriDelta - sweetSpot) * 2;
        }
        if (countTriDeltAvg === 3) {
            avgTriDelta = tempTriDelt / 3.0;
            countTriDeltAvg = 0;
            tempTriDelt = 0.0;
        } else {
            tempTriDelt += leftDistance;
            countTriDeltAvg++;   
        }
        if (travelDirection > 0) {
            SetSpeed(left, 1.0);
            SetSpeed(right, pow(1 - travelDirection, 1.8));
        }
        // if (!lastPressed && GetPin(PIN_F4) && testSpeed < 1.0) {
        //     lastPressed = true;
        //     testSpeed += .01;
        //     Printf("Increasing Speed to %f\n", testSpeed);
        // } else if (!lastPressed && GetPin(PIN_F0) & testSpeed > -1.0) {
        //     lastPressed = true;
        //     testSpeed -= .01;
        //     Printf("Decreasing Speed to %f\n", testSpeed);
        // } else if (lastPressed && !GetPin(PIN_F0) && !GetPin(PIN_F4)) {
        //     lastPressed = false;
        // }
        // Normalize speeds to -0.38 to 0.38;
        // 
        // SetMotor(right, testSpeed);
        // if(!lastTriggered && GetPin(PIN_A2)) {
        //     lastTriggered = true;
        //     Printf("Right: %f Left: %f\n", rightInput, leftInput);
        // } else if (lastTriggered && !GetPin(PIN_A2)) {
        //     lastTriggered = false;
        // }
        // .6 - right/leftInput (+ for move closer to wall, - for move further)
        // To move closer to wall turn left, right motor more power, left motor less power
        // To move further to wall turn right, left motor more power, right motor less power
        // (.6 - input) * 3 = amount to change between motor
        // negative amt = further, set right motor to 1-abs(.6-input)
        // positive amt = closer, set left motor to 1-abs(.6-input)
        // float error = .6 - leftInput;
        // float power = pow(1.0 - fabs(error), 1.6);
        // if (error < 0) {
        //     Printf("Taking %f power from right motor, move further from wall\n", power);
        //     SetSpeed(left, 1.0);
        //     SetSpeed(right, power);
        // } else {
        //     Printf("Taking %f power from left motor, move closer to wall\n", power);
        //     SetSpeed(right, 1.0);
        //     SetSpeed(left, power);
        // }
    }
}

