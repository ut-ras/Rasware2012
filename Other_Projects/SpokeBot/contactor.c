#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

#include "RASLib/servo.h"


//*******************************************************
//  Initializes pin for signal test on crane to
//      determine state of disk pick-up
//*******************************************************
void contactor_init(void) {
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
unsigned char contactor_is_grabbed(void) {
    //Test Input
    if(GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_2) != 0) {
        // Pin High
        return 1;
    } else {
        // Pin Low
        return 0;
    }
}
