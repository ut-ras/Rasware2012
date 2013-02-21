#ifndef SERVO_H
#define SERVO_H

void servo_init(void);
int servo_set(int num, unsigned long in);
int servo_get(int num, unsigned long *out);

#endif
