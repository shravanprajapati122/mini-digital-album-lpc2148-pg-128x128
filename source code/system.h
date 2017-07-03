/*--------------------------------------------------------------
 File:      system.c
 Purpose:   Header file for system driver
 Compiler:  IAR EW 5.5
 System:    Philips LPC2148 MCU with ARM7TDMI-s core
----------------------------------------------------------------*/

//function prototype
void clock_init(void);   

#define PLOCK 0x00000400

void setupPLL0(void);
void feedSeq(void);
void connectPLL0(void);
