#include "inc/hw_types.h"		// tBoolean
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/uart.h"		// input/output over UART
#include "RASLib/init.h"

#include "comm.h"
#include "motor.h"
#include "servo.h"
#include "io.h"

int main(void) {	  	 
	LockoutProtection();
	InitializeMCU();
	
	comm_init();
	motor_init();
	servo_init();
	io_init();
	
	while (1) {
		char cmd = comm_get();
		
		if (cmd == 'c') {
			comm_check();
		} else if (cmd == 'r') {
			int (*dev)(int, unsigned long *);
			unsigned long value;
			int ind;
			
			switch (comm_get()) {
				case 'a': dev = io_a_get; break;
				case 'b': dev = io_b_get; break;
				case 'd': dev = io_d_get; break;
				case 's': dev = servo_get; break;
				case 'm': dev = motor_get; break;
				default: comm_error(); continue;
			}
				
			ind = comm_get()-'0';
			
			if (dev(ind, &value))
				comm_success_read(value);
			else
				comm_error();
				
		} else if (cmd == 'w') {
			int (*dev)(int, unsigned long);
			unsigned long value;
			int ind;
			
			switch (comm_get()) {
				case 'a': dev = io_a_set; break;
				case 'b': dev = io_b_set; break;
				case 'd': dev = io_d_set; break;
				case 's': dev = servo_set; break;
				case 'm': dev = motor_set; break;
				default: comm_error(); continue;
			}
				
			ind = comm_get()-'0';
			
			if (!comm_count(&value)) {
				comm_error();
				continue;
			}
			
			if (dev(ind, value))
				comm_success_write();
			else
				comm_error();
			
		} else {
			comm_error();
		}
	}
}
