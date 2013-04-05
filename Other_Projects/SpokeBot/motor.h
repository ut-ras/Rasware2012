#ifndef MOTOR_H
#define MOTOR_H

// Initialize motors
void motor_init(void);

// Set left motor power
void motor_left(signed char val);

// Set right motor power
void motor_right(signed char val);

// Stop the motors
void motor_stop(void);

// Make them go
void motor_go(signed char speed, signed long left, signed long right);

#endif
