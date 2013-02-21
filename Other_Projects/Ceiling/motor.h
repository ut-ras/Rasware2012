#ifndef MOTOR_H
#define MOTOR_H

void motor_init(void);
int motor_set(int num, unsigned long in);
int motor_get(int num, unsigned long *out);

#endif
