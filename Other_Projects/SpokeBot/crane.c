#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

#include "RASLib/timer.h"

#include "servo.h"

//Servo 1 is for lazy susan base
//Servo 2 is arm

void crane_init(void) 
{
	
}

void crane_pickup(void) 

{
	
	//Servo 1 set to 180 offset of 0
	//Servo 2 set to down until contact limit switch
	//Servo 2 up to original position + x
	//Servo 1 set to 0
	//Servo 2 down by x 

	
	servo_set(1, 0);
	WaitUS(2000000);
	servo_set(1,100);
	WaitUS(2000000);
	servo_set(1,-100);
	
	
}
