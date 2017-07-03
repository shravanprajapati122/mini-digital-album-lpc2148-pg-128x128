#include "buttons.h"

volatile unsigned char power = 1, next_pending = 1, slide_show = 1;

void button_init(void)
{
	 IO0DIR |= 1; // power led
	 IO0SET |= 1;
	
   PINSEL0 |= 1<<29;  //select external interrupt function on pin P0.14 (EINT 1)
	
   PINSEL1 |= 0x301;   //select external interrupt function on pin P0.16 (EINT 0) and pin P0.20 (EINT 3)
  
   EXTINT = 0x0F;        //clear any pending external interrupt requests
   
   EXTMODE = 0x0B;       //external interrupt 0, 1 and 3 set to edge triggered
  
   EXTPOLAR = 0x00;      //and falling edge is selected meaning interrupt is triggered on button release
  
   INTWAKE = 0x02;       //configure external interrupt 1 on P0.14 to wake up the MCU from sleep
  
	 
	 VICIntSelect &= ~(0x0B<<14);		//select Eint0,1,3 as IRQ
	
	
   VICVectAddr0 = (unsigned )power_button_ISR; //Pointer Interrupt Function (ISR)

	 VICVectCntl0 = 0x20|0x0f; // bit15 = 1 -> to enable Eint1					
	
	 VICVectAddr1 = (unsigned )play_button_ISR; //Pointer Interrupt Function (ISR)

	 VICVectCntl1 = 0x20 | 0x0E; // bit14 = 1 -> to enable Eint0
					
	 VICVectAddr2 = (unsigned)next_button_ISR; //Pointer Interrupt Function (ISR)

	 VICVectCntl2 = 0x20 | 0x11; // bit17 = 1 -> to enable Eint3
					
					
	 VICIntEnable |= 0x0B<<14; //Enable interrupts
	 EXTINT &= ~(0x0B);
}

/*-------------------------------------------------------------------------
   Function Name: button_ISR

   Parameters: None
 
   Return    :  None
 
   Description: An interrupt service routine (ISR) to handle the external interrupt
                request by the button
 ---------------------------------------------------------------------------*/

__irq void play_button_ISR(void)
{  
		// clear the interrupt flag; future interrupts won't be recognized otherwise
		EXTINT |= 0x1;
	  if(power){    // otherwise simulation shows continious warnings for trying to access T0TCR
					slide_show = slide_show ? 0 : 1;
					T0TCR = slide_show ? 0x01 : 0x02;   // if slide show enable timer else reset it
		}
		VICVectAddr = 0x00;			
}

__irq void next_button_ISR(void)
{  
	 EXTINT |= 0x8;
	   next_pending = 1;
		 if(slide_show) { // if slide show restart timer
			 T0TCR = 0X02;  // Reset timer
	     T0TCR = 0x01; //Enable timer
		 }
	 VICVectAddr = 0x00;
}

__irq void power_button_ISR(void)
{  
	 EXTINT |= 0x2;
	if(power){
		  power = 0;
		  IO0CLR |= 1 | (1<<21);   // turn off power led and lcd
      PCONP = 0;   // disable all timers, UART, I2C, SPI, ADC, etc modules
      PCON = 0x2;  //go to power down mode
   
   /*Processor is asleep now; the only way to wake it up is using interrupts or reset.
     in this state the chip typically consumes aroung 2uW - this means the system can stay
     in this state for more than two years on a standard 1.5V 'AA' size battery!
   */
	}
	else
	{
		PCONP |= 0X102; // ENABLE TIMER 0 and SPI 0
		power = 1;
		IO0SET |= 1 | (1<<21);
	}
	 VICVectAddr = 0x00;
}
