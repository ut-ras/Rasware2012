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
	
	//VALUES NOT YET TESTED

	
	servo_set(1, 127);     //Servo 1 set to 180 offset of 0
	WaitUS(2000000);       
	servo_set(2, -100);    //Servo 2 set to down until contact limit switch
	WaitUS(2000000);
	servo_set(2,120);     //Servo 2 up to original position + x
	WaitUS(2000000);
	servo_set(1, -128);   //Servo 1 set to 0
	WaitUS(2000000);
	servo_set(2, -20);    //Servo 2 down by x 
	
	
}
