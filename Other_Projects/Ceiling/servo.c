#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

#include "RASLib/servo.h"

#include "servo.h"


const static servo_t SLOOKUP[4] = {SERVO_0, SERVO_1,	SERVO_2, SERVO_3};

void servo_init(void) {
	InitializeServos();
	
	// Enable adc reading of all four for simplicity
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
  //Create a sequence that reads each channel
	ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC_BASE, 0, 0, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC_BASE, 0, 1, ADC_CTL_CH1);
	ADCSequenceStepConfigure(ADC_BASE, 0, 2, ADC_CTL_CH2);
	ADCSequenceStepConfigure(ADC_BASE, 0, 3, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH3);
//	ADCIntEnable(ADC_BASE, 0);
//	IntEnable(INT_ADC0SS0);
	ADCSequenceEnable(ADC_BASE, 0);
}

int servo_set(int num, unsigned long in) {
	if (num < 0 || num >= 4) return 0;
	SetServoPosition(SLOOKUP[num], in & 0xff);
	return 1;
}

int servo_get(int num, unsigned long *out) {
	unsigned long temp[8];
	
	if (num < 0 || num >= 4) return 0;
	
	// Up to 8 values can be buffered
	// Pull all values each time to avoid confusing the buffer
	ADCProcessorTrigger(ADC_BASE, 0);
	while (!ADCIntStatus(ADC_BASE, 0, false));
	ADCIntClear(ADC_BASE, 0);
	ADCSequenceDataGet(ADC_BASE, 0, temp);
	
	*out = temp[num];
	return 1;
}
