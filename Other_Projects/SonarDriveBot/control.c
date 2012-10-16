#include "inc/hw_types.h"		// tBoolean
#include "utils/uartstdio.h"	// input/output over UART

#include "control.h"

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"


volatile unsigned long potValues[2];
volatile int switchValue;

static enum {READY, WAIT} status;
static void (*callback)(unsigned long *, int);

void ControlInit(void) {
	  //GPIO B pin 2 is for the switch
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_2);

    //ADC 0, 1 for inputs
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
    //Create a sequence that reads each channel
    //then triggers and interrupt that calls ADCIntHandler
	ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC_BASE, 0, 0, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC_BASE, 0, 1, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH1);
	ADCIntEnable(ADC_BASE, 0);
	IntEnable(INT_ADC0SS0);
	ADCSequenceEnable(ADC_BASE, 0);
    //Then begin the sequence
	ADCProcessorTrigger(ADC_BASE, 0);	
}

void ADCIntHandler() {
	unsigned long temp[8];
	ADCIntClear(ADC_BASE, 0);
	
    //The sequence data can be up to 8 but only pull 2
	ADCSequenceDataGet(ADC_BASE, 0, temp);
	potValues[0] = temp[0];
	potValues[1] = temp[1];
	
	switchValue = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_2);	
	status = READY;
		
  if (callback) (*callback)((unsigned long *)potValues, switchValue);
}

unsigned long *PotRead(void) {
	ControlBackgroundRead(0);
	while (status != READY);
	return (unsigned long *)potValues;
}

int SwitchRead(void) {
	return (switchValue = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_2));
}

void ControlBackgroundRead(void (*cb)(unsigned long *, int)) {
	callback = cb;
	status = WAIT;
	ADCProcessorTrigger(ADC_BASE, 0);
}	
