#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

#include "RASLib/motor.h"

#include "motor.h"

#define TICKS_DEGREE 5
#define ERR 5
#define kp_L 0.83
#define kp_R 0.83
#define DIST 0.00166
#define SCALE 0


void motor_init(void) {
	InitializeMotors(false), false);
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

void motor_turn(short degrees) {
	
	int encoder0;
	int encoder1; 
	int speed_L = 0;
	int speed_R = 0;
	int pos = degree * TICKS_DEGREE;
	PresetEncoderCounts(0, 0);
	InitializeEncoders(false, true);
	encoder0 = GetEncoderCount(ENCODER_0);
	encoder1 = GetEncoderCount(ENCODER_1);
	while((encoder0) > (pos+ERR) || (encoder0< pos-ERR)){ 
		    if(encoder0 > (pos+ERR)){
			   speed_L = -(100+SCALE);
			   speed_R = -(speed_L);
			}

		    else if(encoder0 < (pos-ERR)) {
			   speed_L = 100;
	     		   speed_R = -(speed_L+SCALE);
		    }
		    SetMotorPowers(speed_L,speed_R); 
		    //UARTprintf("Left Motor = %d, Right Motor = %d\r\n", speed_L, speed_R);
	            //UARTprintf("encoder0 = %d, encoder1 = %d \r\n", encoder0, encoder1); 
		    encoder0 = GetEncoderCount(ENCODER_0);
	            encoder1 = GetEncoderCount(ENCODER_1);									   
	}	
        SetMotorPowers(0,0);
  	//UARTprintf("Done\r\n");
	//Wait(10000);
}

void motor_forward(unsigned char distance){
	
	
}
