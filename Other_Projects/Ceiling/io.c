#include "inc/hw_types.h"		// tBoolean
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

#include "io.h"


const static int PSTART[3] = {2, 2, 2};
const static int PEND[3] = {5, 3, 7};
const static unsigned long PLOOKUP[3] = {
	GPIO_PORTA_BASE, 
	GPIO_PORTB_BASE, 
	GPIO_PORTD_BASE
};

static unsigned char write_enable[3] = {0, 0, 0};

void io_init(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, 0x3c);
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, 0x0c);
	GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, 0xfc);
}

int io_set(int port, int num, unsigned long in) {
	unsigned char pin = 1 << num;
	
	if (num < PSTART[port] || num > PEND[port]) return 0;
	if (!(write_enable[port] & pin)) {
		GPIOPinTypeGPIOOutput(PLOOKUP[port], pin);
		write_enable[port] |= pin;
	}
	
	GPIOPinWrite(PLOOKUP[port], pin, in ? pin : 0);
	return 1;
}

int io_get(int port, int num, unsigned long *out) {
	unsigned char pin = 1 << num;
	
	if (num < PSTART[port] || num > PEND[port]) return 0;
	if (write_enable[port] & pin) {
		GPIOPinTypeGPIOInput(PLOOKUP[port], pin);
		write_enable[port] &= ~pin;
	}
	
	*out = GPIOPinRead(PLOOKUP[port], pin) >> num;
	return 1;
}

int io_a_set(int num, unsigned long in)   { return io_set(0, num, in);  }
int io_a_get(int num, unsigned long *out) { return io_get(0, num, out); }
int io_b_set(int num, unsigned long in)   { return io_set(1, num, in);  }
int io_b_get(int num, unsigned long *out) { return io_get(1, num, out); }
int io_d_set(int num, unsigned long in)   { return io_set(2, num, in);  }
int io_d_get(int num, unsigned long *out) { return io_get(2, num, out); }
