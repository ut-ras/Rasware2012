#include "inc/hw_types.h"		// tBoolean
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/uart.h"		// input/output over UART
#include "RASLib/init.h"

#include "comm.h"
#include "motor.h"
#include "crane.h"

// The main function simply initializes all of the 
// devices that will be used. Then it enters a loop
// that checks comm for incomming data over UART to 
// respond to

// Most message over UART should be a command and value pair:
// 	m - set left motor
// 	n - set right motor
// 	c - sets the crane state
// 	p - ping the rasboard, responds with c
// 	r - reads the value sensor (will be implemented if needed)
//  ! - reset everything
// Responses can be:
// 	c - success
//  r_ - successful read with data
//  x - failure

int main(void) {	  	 
	LockoutProtection();
	InitializeMCU();
	
	comm_init();
	motor_init();
	crane_init();
	
	while (1) {
		unsigned char cmd = comm_read();
		
		switch (cmd) {
			case 'm': // left motor
				motor_left(comm_read());
				comm_write('c');
				break;
			case 'n': // right motor
				motor_right(comm_read());
				comm_write('c');
				break;
			case 'c': // crane
				crane_set(comm_read());
				comm_write('c');
				break;
			case 'p': //ping
				comm_write('c');
				break;
			case '!': //
				motor_left(0);
				motor_right(0);
				crane_set(0);
			  comm_write('c');
				break;
			default:
				comm_write('x');
				break;
		}
	}
}
