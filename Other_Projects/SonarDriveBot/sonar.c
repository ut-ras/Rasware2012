#include "inc/hw_types.h"
#include "utils/uartstdio.h"

#include "sonar.h"

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#include "RASLib/init.h"

#define US(x) ((x)*g_ulTicksPerUs)
#define MS(x) (US(x)*1000)
#define MAX_SONAR_TIME MS(36)

volatile unsigned long sonarValues[2];

static volatile enum { READY, PULSE, WAIT, TIMING, DELAY, DELAY_PULSE } status;
static volatile int current = 0;
static void (*callback)(unsigned long *) = 0;


static void BeginSonarSequence(void) {
	status = PULSE;
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_4, current ? GPIO_PIN_2 : GPIO_PIN_4);

  TimerLoadSet(TIMER2_BASE, TIMER_A, US(8));
  TimerEnable(TIMER2_BASE, TIMER_A);
}

void SonarTimerIntHandler(void) {
	TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
  switch (status) {
	case PULSE:
		status = WAIT;
		GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_4, 0);

    TimerLoadSet(TIMER2_BASE, TIMER_A, MAX_SONAR_TIME);
    TimerEnable(TIMER2_BASE, TIMER_A);
    break;

	case DELAY:
		status = READY;
	  break;

  case DELAY_PULSE:
    BeginSonarSequence();
    break;

  default:
		sonarValues[current] = (unsigned long)-1;
		current = !current;
		status = READY;
	
		if (!current) {
				if (callback) (*callback)((unsigned long*)sonarValues);
		} else {
			SonarBackgroundRead(callback);		
		}
    break;
  }
}

void SonarGPIOIntHandler(void) {
	GPIOPinIntClear(GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_3);
	if (GPIOPinRead(GPIO_PORTD_BASE, current ? GPIO_PIN_3 : GPIO_PIN_5)) {
		status = TIMING;
	
    TimerLoadSet(TIMER2_BASE, TIMER_A, MAX_SONAR_TIME);
    TimerEnable(TIMER2_BASE, TIMER_A);
	} else {
		sonarValues[current] = TimerValueGet(TIMER2_BASE, TIMER_A);
		current = !current;
		status = DELAY;
		
		if (!current) {
				if (callback) (*callback)((unsigned long*)sonarValues);
		} else {
				SonarBackgroundRead(callback);
		}
        
    TimerLoadSet(TIMER2_BASE, TIMER_A, MS(10));
    TimerEnable(TIMER2_BASE, TIMER_A);
	}
}


void SonarInit() {
	// initialize gpio
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	
	GPIOPinTypeGPIOInput (GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_3);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_2);
	
	GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_3, GPIO_BOTH_EDGES);
	GPIOPinIntEnable(GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_3);
	
	IntEnable(INT_GPIOD);
	
	//initialize timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
	TimerConfigure(TIMER2_BASE, TIMER_CFG_32_BIT_OS);
	
	IntEnable(INT_TIMER2A);
	TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
	
	IntMasterEnable();
	status = READY;
}

unsigned long *SonarRead(void) {
	SonarBackgroundRead(0);
	while(status != DELAY && status != READY);
	return (unsigned long*)sonarValues;
}

void SonarBackgroundRead(void (*cb)(unsigned long *)) {
	
	callback = cb;

	if (status == READY)
		BeginSonarSequence();
	else if (status == DELAY)
		status = DELAY_PULSE;
}
