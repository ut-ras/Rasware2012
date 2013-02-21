#ifndef IO_H
#define IO_H

void io_init(void);
int io_set(int port, int num, unsigned long in);
int io_get(int port, int num, unsigned long *out);

int io_a_set(int num, unsigned long in);
int io_a_get(int num, unsigned long *out);
int io_b_set(int num, unsigned long in);
int io_b_get(int num, unsigned long *out);
int io_d_set(int num, unsigned long in);
int io_d_get(int num, unsigned long *out);
	
#endif
