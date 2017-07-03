#include "buttons.h"  // for importing slide_show and next_pending flags
#include "timer.h"
#include <lpc214x.h>

void timer_init()
{
	/*Assuming that PLL0 has been setup with CCLK = 48Mhz and PCLK also = 48Mhz.*/
   
	//----------Configure Timer0-------------

	T0CTCR = 0x0;									   	
	
	T0PR = PRESCALE-1; //(Value in Decimal!) - Increment T0TC at every 48000 clock cycles
                     //Count begins from zero hence subtracting 1
                     //48000 clock cycles @48Mhz = 1 mS
	
	T0MR0 = DELAY_MS-1; //(Value in Decimal!) Zero Indexed Count - hence subtracting 1
	
	T0MCR = MR0I | MR0R; //Set bit0 & bit1 to High which is to : Interrupt & Reset TC on MR0  

	//----------Setup Timer0 Interrupt-------------

  	
	VICVectAddr3 = (unsigned )T0ISR; //Pointer Interrupt Function (ISR)

	VICVectCntl3 = 0x20 | 4; //0x20 (i.e bit5 = 1) -> to enable Vectored IRQ slot
				  //4 is for timer0
	
	VICIntEnable |= 0x10; //Enable timer0 int
	
	T0TCR = 0x02; //Reset Timer
	T0TCR = 0x01; //Enable timer
}

__irq void T0ISR(void)
{
	T0IR = 0xff;

        next_pending = 1;
	
	VICVectAddr = 0x00; //This is to signal end of interrupt execution
}
