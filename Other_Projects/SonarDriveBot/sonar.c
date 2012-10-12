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

volatile unsigned long sonarValue = 0;

static volatile enum { READY, PULSE, WAIT, TIMING, DELAY, DELAY_PULSE } status;
static void (*callback)(unsigned long) = 0;


static void BeginSonarSequence(void) {
	status = PULSE;
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_PIN_2);

    TimerLoadSet(TIMER2_BASE, TIMER_A, US(8));
    TimerEnable(TIMER2_BASE, TIMER_A);
}

void SonarTimerIntHandler(void) {
	TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    switch (status) {
	case PULSE:
		status = WAIT;
		GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);

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
		sonarValue = (unsigned long)-1;
		status = READY;
		if (callback) (*callback)(sonarValue);
        break;
    }
}

void SonarGPIOIntHandler(void) {
	GPIOPinIntClear(GPIO_PORTD_BASE, GPIO_PIN_3);
	if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_3)) {
		status = TIMING;
	
        TimerLoadSet(TIMER2_BASE, TIMER_A, MAX_SONAR_TIME);
        TimerEnable(TIMER2_BASE, TIMER_A);
	} else {
		sonarValue = TimerValueGet(TIMER2_BASE, TIMER_A);
		status = DELAY;
		if (callback) (*callback)(sonarValue);
        
        TimerLoadSet(TIMER2_BASE, TIMER_A, MS(10));
        TimerEnable(TIMER2_BASE, TIMER_A);
	}
}


void SonarInit(void) {
	// initialize gpio
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypeGPIOInput (GPIO_PORTD_BASE, GPIO_PIN_3);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_2);
	
	GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_BOTH_EDGES);
	GPIOPinIntEnable(GPIO_PORTD_BASE, GPIO_PIN_3);
	IntEnable(INT_GPIOD);
	
	//initialize timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
	TimerConfigure(TIMER2_BASE, TIMER_CFG_32_BIT_OS);
	
	IntEnable(INT_TIMER2A);
	TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
	
	status = READY;
}

unsigned long SonarRead(void) {
	SonarBackgroundRead(0);
	while(status != DELAY && status != READY);
	return sonarValue;
}

void SonarBackgroundRead(void (*cb)(unsigned long)) {
	callback = cb;

	if (status == READY)
		BeginSonarSequence();
	else if (status == DELAY)
		status = DELAY_PULSE;
}
