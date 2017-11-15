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

//Algorithm for following walls
// float wallFollowing(float avg100Delta);


// Makes an LED on PIN_F3 blink
void blink(void) {
    SetPin(PIN_F3, blink_on);
    blink_on = !blink_on;
}


// Sets the power of a specified motor 
void SetSpeed(tMotor* motor, float power) {
    float nErr = 0.38;
    SetMotor(motor, power * nErr);
}


// Makes a right turn
void turnRight(float degreeOfTurn, tMotor* left, tMotor* right){
    // SetSpeed(left, 0.0);
    // SetSpeed(right, 0.0);
    SetSpeed(left, 1.0);
    SetSpeed(right, pow(1 - degreeOfTurn, 1.8));
}


// Makes a left turn
void turnLeft(float degreeOfTurn, tMotor* left, tMotor* right){
    // SetSpeed(left, 0.0);
    // SetSpeed(right, 0.0);
    SetSpeed(left, pow(1 + degreeOfTurn, 1.8));
    SetSpeed(right, 1.0);
}


// Return turnDirection and avg of reading from sensors
float lineFollowing(tLineSensor* line, float lv[], int debug){
    LineSensorReadArray(line, lv);
    float travelDirection = 0.0;
    float weights[] = { 4.0, 3.0, 2.0, 1.0, -1.0, -2.0, -3.0, -4.0 };
    float error = 0.0;
    for (int i = 0; i < 8; i++) {
        if (lv[i] > 1) {
            lv[i] = 1;
        }
        error += weights[i] * lv[i];
    }
    travelDirection = error / 7.0;
    return travelDirection;
}


float wallFollowingCP(float avgDelta[], tADC* left, tADC* right, int debug){
    // Sweet spot is the desired distance from the wall to keep
    float sweetSpot = 0.3;

    float avg100Delta = avgDelta[0];
    // These two bools are used to finish a turn
    tBoolean insideTurn = false;
    tBoolean findWall = false;

    // Negative values = turn left
    // Positive values = turn right
    float travelDirection = 0.0; 

    float leftDistance  = ADCRead(left);
    float frontDistance = ADCRead(right);

    // Drastic change in distance => we need to turn to keep up with wall, set findWall
    // Once we start turning, we need to match the sweetspot but still turn a large amount so we use findWall to ensure this
    if (avg100Delta - leftDistance > .3 || (findWall && fabs(leftDistance - sweetSpot) > .1)) {
        // By raising to a power < 1, we increase the value of a decimal
        travelDirection = -pow(avg100Delta, .75);
        findWall = true;

    // Try to get in the sweet spot
    } else if (fabs(avg100Delta - sweetSpot) > .1) { 
        travelDirection = (avg100Delta - sweetSpot) * 3;
        findWall = false;

    } else if (findWall) {
        // Stop trying to find the wall when we don't need to anymore
        findWall = false;
    }

    // We turn towards the wall if we get too far
    if ( (frontDistance > .3) || (insideTurn && (frontDistance > .2)) ) {
        insideTurn = true;
        travelDirection = 1.0;

    //We have finished our turn
    } else if (insideTurn) {
        insideTurn = false;
    }

    // Update avg100Delta
    avgDelta[0] = avg100Delta * 99.0 / 100.0 + leftDistance * 1.0 / 100.0;

    return travelDirection;

}

float wallFollowing(float avgDelta[], tADC* left, tADC* right, int debug){
    // Sweet spot is the desired distance from the wall to keep
    float sweetSpot = 0.3;

    float avg100Delta = avgDelta[0];

    // Negative values = turn left
    // Positive values = turn right
    float travelDirection = 0.0; 

    float leftDistance  = ADCRead(left);
    float rightDistance = ADCRead(right);

    // Drastic change in distance => we need to turn to keep up with wall, set findWall
    // Once we start turning, we need to match the sweetspot but still turn a large amount so we use findWall to ensure this
    if (fabs(avg100Delta - sweetSpot) > .1) { 
        travelDirection = min((avg100Delta - sweetSpot) * 3.0, 1);
    }
    // Update avg100Delta
    avgDelta[0] = avg100Delta * 99.0 / 100.0 + leftDistance * 1.0 / 100.0;

    return travelDirection;

}


int main(void) {
    
    //Initializes motors
    tMotor *left = InitializeServoMotor(PIN_B0, false);
    tMotor *right = InitializeServoMotor(PIN_B3, true);

    //Initializes distance sensors
    tADC *disLeft = InitializeADC(PIN_E4);
    tADC *disRight = InitializeADC(PIN_E5);

    //Initializes reflectance array
    tLineSensor *line = InitializeGPIOLineSensor(PIN_E0, PIN_E3, PIN_E2, PIN_E1, PIN_D3, PIN_D2, PIN_D1, PIN_D0);
    float lv[8];
    LineSensorReadArray(line, lv);
    
    // Average 100 Delta is the average of the last 100 changes in distance
    float avg100Delta[1];
    avg100Delta[0] = ADCRead(disLeft);
    
    // A psuedo-timer used to determine when to output readings for debugging
    int printCount = 0;

    // Flag for if a line has been seen
    tBoolean lineSeen = false;
    
    // Initialization code can go here
    CallEvery(blink, 0, 0.1);
    SetSpeed(left, 0.0);
    SetSpeed(right, 0.0);
    Printf("Initializing the death of you\n");

    
    while(true){
        float lineResults = lineFollowing(line, lv, printCount);
        float wallResults = wallFollowing(avg100Delta, disLeft, disRight, printCount);
        // 70% of travel direction comes from line following and 30% comes from wall following
        // float travelDirection = ((lineResults[1] * 70.0	) + (wallResults[3] * 30.0)) / 100.0;
        float travelDirection = wallResults;

        // Make appropriate turn
        if (travelDirection > 0) {
            turnRight(travelDirection, left, right);
        } else {
            turnLeft(travelDirection, left, right);
        }
        
        //We have waited long enough, now we can check the sensors
        if (printCount == 500) {
            // Printf("|Left Distance: %.3f|\t|Front Distance: %.3f|\t|", wallResults[2], wallResults[3]);
            Printf("Avg 100 Delt: %.3f|Travel Direction: %.3f|\n", avg100Delta[0], travelDirection);
            Printf("LineSens: %.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n",
                    lv[0], lv[1], lv[2], lv[3], lv[4], lv[5], lv[6], lv[7]);
            printCount = 0;

        //We haven't waited long enough
        } else {
            printCount++;
        }
    }
}
