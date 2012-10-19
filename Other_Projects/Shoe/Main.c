#include "inc/hw_types.h"		// tBoolean
#include "RASDemo.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/uart.h"		// input/output over UART
#include "RASLib/init.h"
#include "RASLib/adc.h"
#include "RASLib/motor.h"
#include "driverlib/adc.h"
#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/uart.h"		// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

void InitAdcPorts(void);
long sampleAdcPort(int port);

int main(void)
{	
	LockoutProtection();
	InitializeMCU();
	initUART();																							    
	
	InitAdcPorts();
	InitializeMotors(false, false);
	while (true){
		long port0=sampleAdcPort(0);
		long port1=sampleAdcPort(1);
		int power0=0;
		int power1=0;
		if(port0<200){
			power0=120;
		}
		else if(port0<300){
			power0=100;
		}
		else if(port0<400){
			power0=90;
		}
		else if(port0<450){
			power0=60;
		}
		else if(port0<500){
			power0=30;
		}
		if(port0>500){
			power0=-40;
		}
		if(port0>600){
			power0=-80;
		}
		
		if(port1<200){
			power1=85;
		}
		else if(port1<300){
			power1=70;
		}
		else if(port1<400){
			power1=50;
		}
		else if(port1<450){
			power1=20;
		}
		else if(port1<500){
			power1=0;
		}
		if(port1>500){
			power1=-10;
		}
		if(port1>600){
			power1=-25  ;
		}
		SetMotorPowers(power0,power1);
		
	}
/*	char ch;	  	 

	while(1) {	
		UARTprintf("\nRAS Demo for Robotathon 2012\n");
		UARTprintf("  0=UART Demo\n  1=Motor Demo\n");
		UARTprintf("  2=Servo Demo\n  3=Line Sensor\n");
		UARTprintf("  4=IR Sensor Demo\n  5=Encoders Demo\n");
		
		UARTprintf(">> ");
		ch = getc();
		putc(ch);
		UARTprintf("\n");

		if (ch == '0') {
			UARTprintf("\nUART Demo\n");
			uartDemo();	 
		}
		else if (ch == '1') {
			UARTprintf("\nMotor Demo\n");
			initMotors();
			motorDemo(); 
		}
		else if (ch == '2') {
			UARTprintf("\nServo Demo\n");
			initServo();
			servoDemo();   
		}
		else if (ch == '3') {			   
			UARTprintf("\nLine Sensor Demo\n");
			initLineSensor();		  
			lineSensorDemo();
		}
		else if (ch == '4') {	   
			UARTprintf("\nIR Sensor Demo\n");
			initIRSensor();
			IRSensorDemo();	 
		}
		else if (ch == '5') {
			UARTprintf("\nEncoders Demo\n");
			initEncoders();
			encoderDemo();

		}
	}
*/}



void InitAdcPorts(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
	ADCSequenceConfigure(ADC_BASE,0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC_BASE, 0, 0, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC_BASE, 0, 1, ADC_CTL_CH1);
	ADCSequenceStepConfigure(ADC_BASE, 0, 2, ADC_CTL_CH2);
	ADCSequenceStepConfigure(ADC_BASE, 0, 3, ADC_CTL_CH3 | ADC_CTL_IE | ADC_CTL_END);
	ADCSequenceEnable(ADC_BASE, 0);
}

long sampleAdcPort(int port) {
	unsigned long ADCValues[4] = {0};
	ADCProcessorTrigger(ADC_BASE, 0 ); 
	while(!ADCIntStatus(ADC_BASE, 0, false)); 
	ADCSequenceDataGet(ADC_BASE, 0, ADCValues);
	ADCIntClear(ADC_BASE, 0);
	return ADCValues[port];
}
