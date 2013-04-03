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

// Turn by given degrees
void motor_turn(signed char degrees);

// Drive forward by distance given in inches
// Make sure to compensate for motor drift here
void motor_forward(unsigned char distance);

#endif
