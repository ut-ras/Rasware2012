#ifndef COMM_H
#define COMM_H

void comm_init(void);
char comm_get(void);
int comm_count(unsigned long *out);
void comm_write(const char *message, int count);
void comm_success_read(unsigned long val);
void comm_success_write(void);
void comm_check(void);
void comm_error(void);

#endif
