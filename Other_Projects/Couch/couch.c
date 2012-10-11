#define DEBUG 0
#define USE_LED 1
#define MIN_VAL 30
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

volatile int power, mode;
									
volatile signed long forward_l, sideward_l;
volatile signed long forward_r, sideward_r;

signed char flip0, flip1;


void init_motors() {
		//Initialize Motors
	InitializeServos();

#if USE_LED	
		//Initialize LED Output
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4);
#endif
}

void set_motors(signed char m0, signed char m1) {	
	#if USE_LED
		//Set the LED output
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4, 
							 (m0>MIN_VAL ? 0x10:0x00) | (m0<-MIN_VAL ? 0x20:0x00) | (m1>MIN_VAL ? 0x80:0x00) | (m1<-MIN_VAL ? 0x40:0x00));
#endif
    //invert if necessary
	m0 *= (INV_L?-1:1);
	m1 *= (INV_R?-1:1);
		//Set the servo output
	SetServoPosition(SERVO_0, m0+128);
	SetServoPosition(SERVO_1, m1+128);
}

void gpiod_handler() {
	long val = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_3 | GPIO_PIN_2);
	GPIOPinIntClear(GPIO_PORTA_BASE, GPIO_PIN_3);
    //Only change mode when power is changed
	power = (val >> 3)&1;
	mode = (val >> 2)&1;
	
#if USE_LED
    //Set status LEDs
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3, mode ? 0x04 : 0x08);
#endif	
}

void adc_handler() {
	unsigned long temp[8];
	ADCIntClear(ADC_BASE, 0);
	
    //The sequence data can be up to 8 values and is buffered
    //but because we pull the values everytime they are updated,
    //there should only ever be 4
	ADCSequenceDataGet(ADC_BASE, 0, temp);
	forward_r  = (signed long)(temp[0]-512);
	sideward_r = (signed long)(temp[1]-512);
	forward_l  = (signed long)(temp[2]-512);
	sideward_l = (signed long)(temp[3]-512);
	
    //Then trigger the ADC sequence again
	ADCProcessorTrigger(ADC_BASE, 0);
}

void init_input() {
    //GPIO A pin 2 is for the power signal
	  //GPIO A pin 3 is for the drive mode
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    //Create an interrupt that triggers and calls gpiod_handler 
    //on both edges of D7 signal
    //drive mode will just be polled
	GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_BOTH_EDGES);
	GPIOPinIntEnable(GPIO_PORTA_BASE, GPIO_PIN_3);
	IntEnable(INT_GPIOA);

    //all ADC are for the controller inputs
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
    //Create a sequence that reads each channel
    //then triggers and interrupt that calls adc_handler
	ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC_BASE, 0, 0, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC_BASE, 0, 1, ADC_CTL_CH1);
	ADCSequenceStepConfigure(ADC_BASE, 0, 2, ADC_CTL_CH2);
	ADCSequenceStepConfigure(ADC_BASE, 0, 3, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH3);
	ADCIntEnable(ADC_BASE, 0);
	IntEnable(INT_ADC0SS0);
	ADCSequenceEnable(ADC_BASE, 0);
    //Then begin the sequence
	ADCProcessorTrigger(ADC_BASE, 0);
    //interrupt handlers are defined in Startup.s
		
#if USE_LED
		//Initialize LED Output
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    //Get current state
	gpiod_handler();
#endif
}


int main() {
	signed long ml, mr;
	signed long sc_for_l, sc_side_l;
	signed long sc_for_r, sc_side_r;
	
	LockoutProtection();
	InitializeMCU();
	InitializeUART();
	IntMasterEnable();

	init_input();
	init_motors();
	
#ifdef USE_LED
    //Power LED
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3);
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0xff);
#endif
	
	UARTprintf("- Hi I am Couch! -");
	
	for (;;) {
#if DEBUG
        //range of values is -512 to 512
		UARTprintf("[ f_l:%3d s_l:%3d f_r:%3d s_r:%3d p:%1d m:%1d]\n",
							 forward_l,sideward_l,forward_r,sideward_r,power,mode);
#endif
		
		if (power) {
            //scale the motors to match MAX_VAL
			sc_for_l = (forward_l * 512)/MAX_VAL;
			sc_side_l = (sideward_l * 512)/MAX_VAL;
			sc_for_r = (forward_r * 512)/MAX_VAL;
			sc_side_r = (sideward_r * 512)/MAX_VAL;
	        
			if (!mode) /* Arcade Style */{
				  //Very simple allows for zero point turning
          //and uses overflow check to prevent large values
				ml += sc_side_r;
				ml += sc_for_r;
				mr -= sc_side_r;
				mr += sc_for_r;
			} else /* Control Style */ {
				  //Even simpler allowing direct control of motors
				ml = sc_for_l;
				mr = sc_for_r;
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
			
		} else {
				//Set motors to off
			set_motors(0, 0);
			
			GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4, 0xff);
			
			while (!power);
		}
	}
}

