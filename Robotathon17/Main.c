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
    tMotor *right = InitializeServoMotor(PIN_B3, true);
    tADC *disLeft = InitializeADC(PIN_E5);
    tADC *disFront = InitializeADC(PIN_B4);
    // tADC *disRight = InitializeADC(PIN_E2);
    // Sweet spot is the desired distance from the wall to keep
    float sweetSpot = 0.3;
    float avgTriDelta = ADCRead(disLeft);
    //These are arbitrary values to be tested and changed. 
    // float maxDist = 50;
    // float minDist = 10;
    // float kP = -0.01;
    // float errorL = ADCRead(disLeft) - maxDist;
    // float errorR = ADCRead(disRight) - maxDist;
    // tBoolean lastPressed = false;

    // Initialization code can go here
    CallEvery(blink, 0, 0.5);
    // Runtime code can go here
    Printf("Hello World!\n");
    int printCount = 0;
    // These two bools are used to finish a turn
    tBoolean insideTurn = false;
    tBoolean findWall = false;
    while (1) {
        float travelDirection = 0.0; // -1 => left +1 => right
        // float rightDistance = ADCRead(disRight);
        float leftDistance  = ADCRead(disLeft);
        float frontDistance = ADCRead(disFront);
        // Drastic change in distance => we need to turn to keep up with wall, set findWall
        // Once we start turning, we need to match the sweetspot but still turn a large amount so we use findWall to ensure this
        if (avgTriDelta - leftDistance > .3 || (findWall && fabs(leftDistance - sweetSpot) > .1)) {
            travelDirection = -pow(avgTriDelta, .75); // By raising to a power < 1, we increase the value of a decimal
            findWall = true;
        } else if (fabs(avgTriDelta - sweetSpot) > .1) { // Try to get in the sweet spot
            travelDirection = (avgTriDelta - sweetSpot) * 3;
            findWall = false;
        } else if (findWall) {
            // Stop trying to find the wall when we don't need to anymore
            findWall = false;
        }
        // Inside turn if the front sensor gets close or if we are in progress of inside turning
        if (frontDistance > .3 || (insideTurn && frontDistance > .2)) {
            insideTurn = true;
            travelDirection = 1.0;
        } else if (insideTurn) {
            insideTurn = false;
        }
        // Update avgTriDelta
        avgTriDelta = avgTriDelta * 2.0 / 3.0 + leftDistance * 1.0 / 3.0;
        // Move to the desired travel direction
        if (travelDirection > 0) {
            SetSpeed(left, 1.0);
            SetSpeed(right, pow(1 - travelDirection, 1.8));
        } else {
            SetSpeed(left, pow(1 + travelDirection, 1.8));
            SetSpeed(right, 1.0);
        }
        if (printCount == 500) {
            Printf("|Left Distance: %.3f|\t|Front Distance: %.3f|\t|", leftDistance, frontDistance);
            Printf("Avg Tri Delt: %.3f|\t|Travel Direction: %.3f|\n", avgTriDelta, travelDirection);
            printCount = 0;
        } else {
            printCount++;
        }
        // if (leftDistance < .1) {
        //     SetSpeed(left, 0.0);
        //     SetSpeed(right, 0.0);
        //     Wait(1.0);
        //     continue;
        // }
    }
}

