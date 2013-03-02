#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

#include "RASLib/servo.h"

void crane_init(void) {
	InitializeServos();
}

void crane_set(unsigned char val) {
	SetServoPosition(SERVO_0, val);
}


