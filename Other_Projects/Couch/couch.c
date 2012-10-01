#define DEBUG
#define USE_RASLIB 1
#define MAX_VAL 350
#define INV_L true
#define INV_R false

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

#define InitializeUART()										\
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);				\
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);	\
	UARTStdioInit(0);


//these variables are volatile because they can be changed in interrupts

volatile tBoolean power;
volatile signed long forward;
volatile signed long sideward;

signed char flip0, flip1;


void init_motors(tBoolean inv0, tBoolean inv1) {
		//Initialize Motors
	InitializeServos();
	
		//Initialize LED Output
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
	GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4);
	
    //Create the PWM signal
	PWMGenConfigure(PWM_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	PWMGenPeriodSet(PWM_BASE, PWM_GEN_1, 256);
	PWMOutputInvert(PWM_BASE, PWM_OUT_4_BIT,  INV_L);
	PWMOutputInvert(PWM_BASE, PWM_OUT_5_BIT, !INV_L);
	PWMOutputInvert(PWM_BASE, PWM_OUT_6_BIT,  INV_R);
	PWMOutputInvert(PWM_BASE, PWM_OUT_7_BIT, !INV_R);
	PWMOutputState(PWM_BASE, PWM_OUT_7_BIT | PWM_OUT_6_BIT | PWM_OUT_5_BIT | PWM_OUT_4_BIT, true);
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_4, 128);
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_5, 128);
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_6, 128);
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_7, 128);
	PWMGenEnable(PWM_BASE, PWM_GEN_1);
}

void set_motors(signed char m0, signed char m1) {
	m0 *= INV_L ? -1 : 1;
	m1 *= INV_R ? -1 : 1;
		//Set the servo output
	SetServoPosition(SERVO_0, m0+128);
	SetServoPosition(SERVO_1, m1+128);
	
		//Set the LED output
		//TODO make this work
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_4, m0+128);
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_5, m1+128);
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_6, m0+128);
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_7, m1+128);
}

void init_input() {
    //GPIO D pin 7 is for the power signal
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_7);
    //Create an interrupt that triggers and calls gpiod_handler 
    //on both edges of D7 signal
	GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_7, GPIO_BOTH_EDGES);
	GPIOPinIntEnable(GPIO_PORTD_BASE, GPIO_PIN_7);
	IntEnable(INT_GPIOD);

    //ADC channel 0 and 1 are for the controller inputs
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
    //Create a sequence that reads both channel 0 and then channel 1
    //then triggers and interrupt that calls adc_handler
	ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC_BASE, 0, 0, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC_BASE, 0, 1, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH1);
	ADCIntEnable(ADC_BASE, 0);
	IntEnable(INT_ADC0SS0);
	ADCSequenceEnable(ADC_BASE, 0);
    //Then begin the sequence
	ADCProcessorTrigger(ADC_BASE, 0);

    //interrupt handlers are defined in Startup.s
}

void gpiod_handler() {
	GPIOPinIntClear(GPIO_PORTD_BASE, GPIO_PIN_7);
	power = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_7) >> 7;
}

void adc_handler() {
	unsigned long temp[8];
	ADCIntClear(ADC_BASE, 0);
	
    //The sequence data can be up to 8 values and is buffered
    //but because we pull the values everytime they are updated,
    //there should only ever be 2
	ADCSequenceDataGet(ADC_BASE, 0, temp);
	forward  = (signed long)(temp[0]-512);
	sideward = (signed long)(temp[1]-512);
	
    //Then trigger the ADC sequence again
	ADCProcessorTrigger(ADC_BASE, 0);
}

void init_display() {
	unsigned char eh = 0;
	
		//Both port D and port B used as led outputs
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3);
	
	
	
	for (;;) {
		GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2, eh);
		GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0xff);
		eh <<= 1;
		if (!eh) eh = 1;
		WaitUS(1000000);
	}
}

int main() {
	signed long ml, mr, sc_for, sc_side;
	
	LockoutProtection();
	InitializeMCU();
	InitializeUART();
	IntMasterEnable();

	init_input();
	init_display();
	
	UARTprintf("- Hi I am Couch! -");
	
	for (;;) {
#ifdef DEBUG
        //range of values is -512 to 512
		UARTprintf("[ f:%d s:%d p:%d ]\n",forward,sideward,power);
#endif
		
		if (!power) {
            //scale the motors to match MAX_VA
			sc_for = (forward * 512)/MAX_VAL;
			sc_side = (sideward * 512)/MAX_VAL;
	        
            //reduce the motor you're turning into
			if (sc_side > 0) {
				ml = (sc_for * (512-sc_side))/512;
				mr = sc_for;
			} else {
				ml = sc_for;
				mr = (sc_for * (512+sc_side))/512;
			}
             
            //shift to put the motors in the range of a char
			ml >>= 2;
			mr >>= 2;
            //check for overflow
			if (ml >  126) ml =  126;
			if (ml < -127) ml = -127;
			if (mr >  126) mr =  126;
			if (mr < -127) mr = -127;
			
			set_motors((signed char)ml, (signed char)mr);
		}
	}
}

