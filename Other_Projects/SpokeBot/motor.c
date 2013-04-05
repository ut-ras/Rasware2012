#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

#include "RASLib/motor.h"
#include "RASLib/encoder.h"

#include "motor.h"


void motor_init(void) {
	InitializeMotors(false, false);
	InitializeEncoders(false, false);
}

void motor_left(unsigned char val) {
	SetMotorPower(MOTOR_0, val);
}

void motor_right(unsigned char val) {
	SetMotorPower(MOTOR_1, val);
}

void motor_stop(void) {
	SetMotorPower(MOTOR_0, 0);
	SetMotorPower(MOTOR_1, 0);
}

void motor_go(signed char speed, signed long left, signed long right) {
	signed long enc_l, enc_r;
	PresetEncoderCounts(0, 0);
	if (left < 0 && right == 0) {
		SetMotorPower(MOTOR_0, -speed);
		do { GetEncoderCounts(&enc_l, &enc_r); } while (enc_l > left);
		
	} else if (left > 0 && right == 0) {
		SetMotorPower(MOTOR_0, speed);
		do { GetEncoderCounts(&enc_l, &enc_r); } while (enc_l < left);
			
	} else if (left == 0 && right < 0) {
		SetMotorPower(MOTOR_1, -speed);
		do { GetEncoderCounts(&enc_l, &enc_r); } while (enc_r > right);
			
	} else if (left == 0 && right > 0) {
		SetMotorPower(MOTOR_1, speed);
		do { GetEncoderCounts(&enc_l, &enc_r); } while (enc_r < right);
	
	} else if (left < 0 && right < 0) {
		SetMotorPower(MOTOR_0, -speed);
		SetMotorPower(MOTOR_1, -speed);
		do { GetEncoderCounts(&enc_l, &enc_r); } while (enc_l > left && enc_r > right);
		
	}	else if (left > 0 && right < 0) {
		SetMotorPower(MOTOR_0, speed);
		SetMotorPower(MOTOR_1, -speed);
		do { GetEncoderCounts(&enc_l, &enc_r); } while (enc_l < left && enc_r > right);
		
	}	else if (left < 0 && right > 0) {
		SetMotorPower(MOTOR_0, -speed);
		SetMotorPower(MOTOR_1, speed);
		do { GetEncoderCounts(&enc_l, &enc_r); } while (enc_l > left && enc_r < right);
		
	} else if (left > 0 && right > 0) {
		SetMotorPower(MOTOR_0, speed);
		SetMotorPower(MOTOR_1, speed);
		do { GetEncoderCounts(&enc_l, &enc_r); } while (enc_l < left && enc_r < right);
	}	
	
	SetMotorPower(MOTOR_0, 0);
	SetMotorPower(MOTOR_1, 0);
}
