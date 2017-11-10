#include <RASLib/inc/common.h> 
#include <RASLib/inc/gpio.h> 
#include <RASLib/inc/time.h> 
#include <RASLib/inc/adc.h>
#include <RASLib/inc/sonar.h> 
#include <RASLib/inc/linesensor.h> 
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
    tADC *disLeft = InitializeADC(PIN_E4);
    tADC *disRight = InitializeADC(PIN_E5);
    SetPin(PIN_A5, true);
    tLineSensor *line = InitializeGPIOLineSensor(PIN_E0, PIN_E3, PIN_E2, PIN_E1, PIN_D3, PIN_D2, PIN_D1, PIN_D0);
    float lv[8]; 
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
    
    SetSpeed(left, 0.0);
    SetSpeed(right, 0.0);

    // Initialization code can go here
    CallEvery(blink, 0, 0.1);
    // Runtime code can go here
    Printf("Initializing the death of you\n");
    int printCount = 0;
    while (1) {
        LineSensorReadArray(line, lv);
        float rightDistance = ADCRead(disRight);
        float leftDistance  = ADCRead(disLeft);
        float power = 0;
        // Drastic change in distance => we need to turn to keep up with wall, set findWall
        // Once we start turning, we need to match the sweetspot but still turn a large amount so we use findWall to ensure this
        if (fabs(avgTriDelta - sweetSpot) > .1) { // Try to get in the sweet spot
            power = pow(avgTriDelta - sweetSpot, .3);
        }
        // Update avgTriDelta
        avgTriDelta = avgTriDelta * 2.0 / 3.0 + leftDistance * 1.0 / 3.0;
        // Move to the desired travel direction
        // SetSpeed(left, pow(1 + power, 1.8));
        // SetSpeed(right, 1.0);
        if (printCount == 500) {
            Printf("|Left Distance: %.3f|\t|Right Distance: %.3f|\t|", leftDistance, rightDistance);
            Printf("Avg Tri Delt: %.3f|\t|Power: %.3f|\n", avgTriDelta, power);
            Printf("%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", lv[0], lv[1], lv[2], 
                lv[3], lv[4], lv[5], lv[6], lv[7]); 
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

