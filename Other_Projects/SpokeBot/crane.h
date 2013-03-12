#ifndef CRANE_H
#define CRANE_H

// Initialize the crane
void crane_init(void);

// Initialize the state test for disk pickup
void isDisk_grabbedInit(void);

// Sets the crane position
// This can be modified for more control if needed
void crane_set(unsigned char val);

// Checks state of disk
tbool isDisk_grabbed(void);

#endif
