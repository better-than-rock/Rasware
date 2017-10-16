#include <RASLib/inc/common.h>
#include <RASLib/inc/gpio.h>
#include <RASLib/inc/time.h>
#include <RASLib/inc/motor.h>

// Blink the LED to show we're on
tBoolean blink_on = true;

void blink(void) {
    SetPin(PIN_F4, blink_on);
    blink_on = !blink_on;
}


// The 'main' function is the entry point of the program
int main(void) {
    // Initialization code can go here
    tMotor *testing = InitializeServoMotor(PIN_B0, true);
    tMotor *testing2 = InitializeServoMotor(PIN_B7, false);
    SetMotor(testing, 1.0);
    SetMotor(testing2, 1.0);
    CallEvery(blink, 0, 0.25);
    
    while (1) {
        // Runtime code can go here
        Printf("Hello World!\n");
        
    }
}
