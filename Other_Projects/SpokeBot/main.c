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

// Supported messages
// 	w[d][val]  - set device motor
// 	p          - ping the rasboard, responds with c
// 	r[d]			 - reads the value sensor (will be implemented if needed)
//  !          - reset everything
// Responses can be:
// 	c          - successful ping
//  a[d]       - successful write
//  d[d]_ 	   - successful read with data
//  x          - failure

enum devices {
	MOTOR_LEFT  = 'm'+0,
	MOTOR_RIGHT = 'm'+1,
	CRANE = 'c'
};

int main(void) {	  	 
	LockoutProtection();
	InitializeMCU();
	
	comm_init();
	motor_init();
	crane_init();
	
	while (1) {
		unsigned char cmd, dev;
		cmd = comm_read();
		
		switch (cmd) {
			case 'w':
				dev = comm_read();
			
				switch (dev) {
					case MOTOR_LEFT:
						motor_left(comm_read());
					
						comm_write('a');
						comm_write(MOTOR_LEFT);
						continue;
					
					case MOTOR_RIGHT:
						motor_right(comm_read());
					
						comm_write('a');
						comm_write(MOTOR_RIGHT);
						continue;
					
					case CRANE:
						crane_set(comm_read());
					
						comm_write('a');
						comm_write(CRANE);
						continue;
					
					default:
						comm_write('x');
						continue;
				}
				
			//case 'r': to be implemented 
				
			case 'p':
				comm_write('c');
				continue;
			
			case '!':
				motor_left(0);
				motor_right(0);
				continue;
			
			default:
				comm_write('x');
		}
	}
}
