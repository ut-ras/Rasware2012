#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

#include "RASLib/servo.h"
#include "RASLib/timer.h"

#include "servo.h"


const static servo_t SLOOKUP[4] = {SERVO_0, SERVO_1,	SERVO_2, SERVO_3};

void servo_init(void) {
	InitializeServos();
}

void servo_set(int num, signed char val) {
		SetServoPosition(SLOOKUP[num], (unsigned char)(val + 128));
}
