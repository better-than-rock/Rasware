#include <RASLib/inc/common.h>
#include <RASLib/inc/gpio.h>

tBoolean value = false;

void InitSwitch(void) {
    InitializeGPIO();
}

void PrintSwitch(tPin pin) {
    value = GetPin(pin);
  	Printf("Switch value is %d\n", value); //%d is the escape character for integers
  	// \n is the escape character for newlines
}
