/*--------------------------------------------------------------
 File:      system.c
 Purpose:   To initialize and configure the ARM Clock source
 System:    Philips LPC2148 MCU with ARM7TDMI-s core
 
 Notes:
     PLL = Phase Locked Loop - Is an electronic circuit consisting of
	                           Current controlled oscillator(CCO), phase detector
 							   and frequency multiplier.
		   The task of the PLL is to generate the required clock for the CPU from
		   the crystal oscillator base frequency.
		   
	 PLL feed code = The byte sequence (0xAA-0x55) is required to change the PLL config.
	                 to protect it from changing inadvertently by spurious system activity
					 or external processes.
----------------------------------------------------------------*/
#include <lpc214x.h>
#include "system.h"
		
void clock_init(void)
{
	
	setupPLL0();
	feedSeq(); //sequence for locking PLL to desired freq.
	connectPLL0();
	feedSeq(); //sequence for connecting the PLL as system clock
	
	//SysClock is now ticking @ 48Mhz!
       
	VPBDIV = 0x01; // PCLK is same as CCLK i.e 48Mhz

}

void setupPLL0(void)
{
    PLL0CON = 0x01; // PPLE=1 & PPLC=0 so it will be enabled 
                    // but not connected after FEED sequence
    PLL0CFG = 0x23; // set the multipler to 4 (i.e actually 3) 
                    // i.e 12x4 = 48 Mhz (M - 1 = 4)!!!
                    // Set P=2 since we want FCCO in range!!!
                    // So , Assign PSEL =01 in PLL0CFG as per the table.
}

void feedSeq(void)
{
	PLL0FEED = 0xAA;
	PLL0FEED = 0x55;
}

void connectPLL0(void)
{
    // check whether PLL has locked on to the  desired freq by reading the lock bit
    // in the PPL0STAT register

    while( !( PLL0STAT & PLOCK ));

	// now enable(again) and connect
	PLL0CON = 0x03;
}
