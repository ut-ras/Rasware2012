#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/uart.h"		// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "comm.h"

void comm_init(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);				
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTStdioInit(0);
}

unsigned char comm_read(void) {
	return UARTgetc();
}

void comm_write(unsigned char val) {
	UARTwrite((const char *)&val, 1);
}
