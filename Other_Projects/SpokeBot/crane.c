#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

#include "RASLib/servo.h"

#define BOOL int
#define TRUE 1
#define FALSE 0

void crane_init(void) {
	InitializeServos();
    isDisk_grabbedInit(void);
}

void crane_set(unsigned char val) {
	SetServoPosition(SERVO_0, val);
}

//*******************************************************
//  Initializes pin for signal test on crane to
//      determine state of disk pick-up
//*******************************************************
void isDisk_grabbedInit(void) {
    //Pin initializations
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_2);
}


//********************************************************
//  Checks input signal from PB-2, if signal high,
//      then disk is in contact with crane
//
//  Inputs: void
//  Outputs: BOOL(int) TRUE(1) if disk connected, FALSE(0) if not
//
//  -->Uses PortB Pin2, call isDisk_grabbedInit before
//      calling this function
//********************************************************
BOOL isDisk_grabbed(void) {
    //Test Input
    if(GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_2) != 0) {
        // Pin High
        return TRUE;
    } else {
        // Pin Low
        return FALSE;
    }
}
