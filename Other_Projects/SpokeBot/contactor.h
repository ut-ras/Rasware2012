#ifndef CONTACTOR_H
#define CONTACTOR_H

//*******************************************************
//  Initializes pin for signal test on crane to
//      determine state of disk pick-up
//*******************************************************
void contactor_init(void);

//********************************************************
//  Checks input signal from PB-2, if signal high,
//      then disk is in contact with crane
//
//  Inputs: void
//  Outputs: unsigned char TRUE(1) if disk connected, FALSE(0) if not
//
//  -->Uses PortB Pin2, call contactor_init before
//      calling this function
//********************************************************
unsigned char contactor_is_grabbed(void);

#endif
