#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

#include "RASLib/motor.h"

#include "motor.h"

void motor_init(void) {
	InitializeMotors(false, false);
}

void motor_left(unsigned char val) {
	SetMotorPower(MOTOR_0, val);
}

void motor_right(unsigned char val) {
	SetMotorPower(MOTOR_1, val);
}
