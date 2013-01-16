#define DEBUG 1

#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

#include "utils/uartstdio.h"	// input/output over UART

#include "driverlib/uart.h"		// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"

#include "RASLib/timer.h"
#include "RASLib/init.h"
#include "RASLib/servo.h"

#include "sonar.h"
#include "control.h"

#define InitializeUART()										\
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);				\
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);	\
	UARTStdioInit(0);

void SpinSonar(unsigned long *eh) {
	SonarBackgroundRead(&SpinSonar);
}

void SpinControls(unsigned long *eh, int ehh) {
	ControlBackgroundRead(&SpinControls);
}


int main() {	
	LockoutProtection();
	InitializeMCU();
	InitializeUART();
	
	SonarInit();
	ControlInit();
	InitializeServos();
	
	SpinSonar(0);
	SpinControls(0,0);
	
	for (;;) {
		unsigned long l_bias, r_bias;
		signed long total_bias;

#ifdef DEBUG
		UARTprintf("[%d, %d] : [%d, %d][%d]", 
							 sonarValues[0], sonarValues[1],
							 potValues[0], potValues[1],
							 switchValue);
#endif
		
		if (switchValue)
			SetServoPosition(SERVO_1, 64);
		else
			SetServoPosition(SERVO_1, 48);
		
		l_bias = sonarValues[0];
		if (l_bias < 1300000) l_bias = 1300000;
		l_bias -= 1300000;
		
		r_bias = sonarValues[1];
		if (r_bias < 1300000) r_bias = 1300000;
		r_bias -= 1300000;
		
		total_bias = 500000 - l_bias + r_bias;
		
		total_bias += (potValues[0] * 250000) / 1024;
		total_bias -= (potValues[1] * 250000) / 1024;	
		total_bias /= (1000000/256);
		
		
		if (total_bias >= 256) total_bias = 255;
		else if (total_bias < 0) total_bias = 0;
		
		SetServoPosition(SERVO_0, total_bias);
	}
}

