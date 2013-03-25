#include "inc/hw_types.h"		// tBoolean
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/uart.h"		// input/output over UART
#include "RASLib/init.h"

#include "comm.h"
#include "motor.h"
#include "crane.h"
#include "contactor.h"
#include "servo.h"

// The main function simply initializes all of the 
// devices that will be used. Then it enters a loop
// that checks comm for incomming data over UART to 
// respond to

// Supported messages
// 	w[d][val]  - set device
// 	p          - ping the rasboard, responds with c
// 	r[d]			 - reads the value (will be implemented if needed)
//  !          - reset everything
// Responses can be:
// 	c          - successful ping
//  a[d]       - successful write
//  d[d][val]  - successful read with data
//  x          - failure

enum {
	MOTOR_LEFT  = 'm',
	MOTOR_RIGHT      ,
	MOTOR_0	 	  = 'm',
	MOTOR_1   	     ,

	SERVO_0 	  = 's',
	SERVO_1 	       ,
	SERVO_2 	       ,
	SERVO_3 	       ,
	
	CONTACTOR 	= 'c',
	CRANE 	    = 'r',
};

int main(void) {	  	 
	LockoutProtection();
	InitializeMCU();
	
	comm_init();
	motor_init();
	servo_init();
	contactor_init();
	crane_init();
	
	crane_pickup();
	while (1);
	
	while (1) {
		unsigned char cmd, dev;
		cmd = comm_read();
		
		switch (cmd) {
			case 'w':
				dev = comm_read();
			
				switch (dev) {
					case MOTOR_LEFT:
						motor_left((signed char)comm_read());
					
						comm_write('a');
						comm_write(MOTOR_LEFT);
						continue;
					
					case MOTOR_RIGHT:
						motor_right((signed char)comm_read());
					
						comm_write('a');
						comm_write(MOTOR_RIGHT);
						continue;
					
					case SERVO_0:
					case SERVO_1:
					case SERVO_2:
					case SERVO_3:
						servo_set(dev-SERVO_0, (signed char)comm_read());
					
					  comm_write('a');
						comm_write(dev);
					  continue;
					
					case CRANE:
						crane_pickup();
					
						comm_write('a');
						comm_write(CRANE);
						continue;
					
					default:
						comm_write('x');
						continue;
				}
				
			case 'r':
				dev = comm_read();
			
				switch (dev) {
					case CONTACTOR:
						comm_write('d');
					  comm_write(contactor_is_grabbed());
						continue;
					
					default:
						comm_write('x');
						continue;
				}
				
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
