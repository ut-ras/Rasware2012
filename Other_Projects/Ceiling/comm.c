#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/uart.h"		// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "comm.h"

char message[5];

void comm_init(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);				
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTStdioInit(0);
}

char comm_get(void) {
	return UARTgetc();
}

int comm_count(unsigned long *out) {
	unsigned long value = 0;
	char rec = comm_get();
	
	while (rec != ';') {
		if (rec < '0' || rec > '9') return 0;
		value = value*10 + (rec-'0');
		
		rec = comm_get();
	}
	
	*out = value;
	return 1;
}

void comm_write(const char *message, int count) {
	UARTwrite(message, count);
}

void comm_success_read(unsigned long val) {
	UARTprintf("r%u;", val);
}

void comm_success_write(void) {
	char msg = 'w';
	UARTwrite(&msg, 1);
}

void comm_check(void) {
	char msg = 'c';
	UARTwrite(&msg, 1);
}

void comm_error(void) {
	char msg = 'x';
	UARTwrite(&msg, 1);
}
