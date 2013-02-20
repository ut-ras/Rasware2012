#ifndef COMM_H
#define COMM_H

// Initializes UART communications
void comm_init(void);

// Reads a single byte of data
unsigned char comm_read(void);

// Writes a single byte of data
void comm_write(unsigned char val);

#endif
