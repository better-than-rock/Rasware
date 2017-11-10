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

//Algorithm for following walls
float wallFollowing(float avg100Delta);

// Makes a right turn
void turnRight(float degreeOfTurn){
	SetSpeed(left, 1.0);
	SetSpeed(right, pow(1 - degreeOfTurn, 1.8));
}


// Makes a left turn
void turnLeft(float degreeOfTurn){
	SetSpeed(left, pow(1 + degreeOfTurn, 1.8));
   	SetSpeed(right, 1.0);
}


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


int main(void) {
	
	//Initializes motors
    tMotor *left = InitializeServoMotor(PIN_B0, false);
    tMotor *right = InitializeServoMotor(PIN_B3, true);

	//Initializes distance sensors
    tADC *disLeft = InitializeADC(PIN_E5);
    tADC *disFront = InitializeADC(PIN_B4);

	//Initializes reflectance array
	tLineSensor *line = InitializeGPIOLineSensor(PIN_B1, PIN_B2, PIN_E4, PIN_E5, PIN_B5, PIN_A5, PIN_A6, PIN_A7);
	float linevals[8];
    tLineSensorReadArray(line, linevals);
    
	// Average 100 Delta is the average of the last 100 changes in distance
    float avg100Delta = ADCRead(disLeft);


	// A psuedo-timer used to determine when to output readings for debugging
	float printCount = 0;

	// Flag for if a line has been seen
	tBoolean lineSeen = false;	
	
	while(true){
		float[] lineResults = lineFollowing();
		float[] wallResults = wallFollowing(avg100Delta);

		//Updates necessary values
		avg100Delta = results[0];
		printCount = results[1];
		
		// 70% of travel direction comes from line following and 30% comes from wall following
		float travelDirection = ((lineResults[1] * 70.0	) + (wallResults[3] * 30.0)) / 100.0;

		// Make appropriate turn
    	if (travelDirection > 0) {
       		turnRight(travelDirection);
    	} else {
        	turnLeft(travelDirection);
    	}
	}
}

// Return turnDirection and avg of reading from sensors
float[] lineFollowing(void){
	//read and store values from the array
	
	//check each value
		//if it's < .50, there is a line
		//if it's > .50, there is no line

}

float[] wallFollowing(float avg100Delta){
	
	// Sweet spot is the desired distance from the wall to keep
    float sweetSpot = 0.3;


	// These two bools are used to finish a turn
    tBoolean insideTurn = false;
    tBoolean findWall = false;

	// Negative values = turn left
	// Positive values = turn right
    float travelDirection = 0.0; 

    float leftDistance  = ADCRead(disLeft);
    float frontDistance = ADCRead(disFront);

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
    avg100Delta = avg100Delta * 99.0 / 100.0 + leftDistance * 1.0 / 100.0;

	//We have waited long enough, now we can check the sensors
    if (printCount == 500) {
        Printf("|Left Distance: %.3f|\t|Front Distance: %.3f|\t|", leftDistance, frontDistance);
        Printf("Avg 100 Delt: %.3f|\t|Travel Direction: %.3f|\n", avg100Delta, travelDirection);
        printCount = 0;

	//We haven't waited long enough
    } else {
        printCount++;
    }

	float[] results = {avg100Delta, printCount, travelDirection};
	return results;
}





