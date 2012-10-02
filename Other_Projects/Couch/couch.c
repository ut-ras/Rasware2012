#define DEBUG 1
#define USE_LED 1
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

volatile tBoolean power, mode;
									
volatile signed long forward_l, sideward_l;
volatile signed long forward_r, sideward_r;

signed char flip0, flip1;


void init_motors() {
		//Initialize Motors
	InitializeServos();

#if USE_LED	
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
#endif
}

void set_motors(signed char m0, signed char m1) {
	m0 *= (INV_L?-1:1);
	m1 *= (INV_R?-1:1);
		//Set the servo output
	SetServoPosition(SERVO_0, m0+128);
	SetServoPosition(SERVO_1, m1+128);
	
#if USE_LED
		//Set the LED output
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_4, m0>0 ? 0     : m0*2);
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_5, m0>0 ? -m0*2 : 0   );
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_6, m1>0 ? 0     : m1*2);
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_7, m1>0 ? -m1*2 : 0   );
#endif
}

void init_input() {
    //GPIO A pin 3 is for the power signal
	  //GPIO A pin 4 is for the drive mode
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_3 | GPIO_PIN_4);
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
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_3 | GPIO_PIN_2);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3);
#endif
}

void gpiod_handler() {
	long val;
	GPIOPinIntClear(GPIO_PORTA_BASE, GPIO_PIN_3);
	val = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_3 | GPIO_PIN_4);
    //Only change mode when power is changed
	power = val >> 3;
	mode = val >> 4;
	
#if USE_LED
    //Set status LEDs
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3 | GPIO_PIN_4, mode ? 0x10 : 0x08);
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, power << 3);
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
	
	UARTprintf("- Hi I am Couch! -");
	
	for (;;) {
#if DEBUG
        //range of values is -512 to 512
		UARTprintf("[ f_l:%3d s_l:%3d f_r:%3d s_r:%3d p:%1d ]\n",forward_l,sideward_l,forward_r,sideward_r,power);
#endif
		
		if (!power) {
            //scale the motors to match MAX_VAL
			sc_for_l = (forward_l * 512)/MAX_VAL;
			sc_side_l = (sideward_l * 512)/MAX_VAL;
			sc_for_r = (forward_r * 512)/MAX_VAL;
			sc_side_r = (sideward_r * 512)/MAX_VAL;
	        
			if (mode) /* Arcade Style */{
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
			set_motors(0, 0);
		}
	}
}

