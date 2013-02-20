#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

void crane_init(void) {
	// TODO: This should initialize everythin the crane control needs
}

void crane_set(unsigned char val) {
	// TODO: This shoule set the crane to some state
	//
	// Finer control could be added if needed
}
