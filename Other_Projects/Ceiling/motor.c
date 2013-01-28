#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

#include "RASLib/motor.h"
#include "RASLib/encoder.h"

#include "motor.h"

const static motor_t MLOOKUP[2] = {MOTOR_0, MOTOR_1};
const static encoder_t ELOOKUP[2] = {ENCODER_0, ENCODER_1};

void motor_init(void) {
	InitializeMotors(false, false);
	InitializeEncoders(false, false);
}

int motor_set(int num, unsigned long in) {
	if (num < 0 || num >= 2) return 0;
	SetMotorPower(MLOOKUP[num], in & 0xff);
	return 1;
}

int motor_get(int num, unsigned long *out) {
	if (num < 0 || num >= 2) return 0;
	*out = GetEncoderCount(ELOOKUP[num]);
	return 1;
}
