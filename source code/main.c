 /*----------------------------------------------------------------------------
    File name   : main.c
    Description : A digital album that retrieves prestored images on a 
		              memory card and displays them on a 128x128 graphic lcd

    Procesor    : Philips LPC2148 MCU with ARM7TDMI-S core
 ----------------------------------------------------------------------------*/
#include <lpc214x.h>
#include "buttons.h"
#include "system.h"
#include "timer.h"
#include "lcd.h"
#include "spi.h"
#include "SD.h"

void initialize(void);
void next(void);

unsigned char img [2048];
unsigned char const MAX_INDEX = 12;
char cur_index = 0;
	
int main()
{
   initialize();
	 while(1)
   {
		 if(next_pending == 1){
			  next_pending = 0;
			  next();
		 }
   }
   return 0;
}

void next() {
	  cur_index = (cur_index + 1) % MAX_INDEX;
		if(SD_readImage(cur_index*4,img))     // if readImage executes correctly it will finally return 0
		{
			 draw_string(0,14, "memory card error"); // Display error message
		}
		else
		{          
				clrram();                         // Clear the screen
				disp_img(0,16,128, img);        // Display the bitmap
		}  
  
	
		if(cur_index < 10)
		{
				wr_xd((16),0x24);
				wr_comm(0xb0);
					 wr_auto(cur_index + 16);
				wr_comm(0xb2);
		}
		else
		{
				wr_xd((16),0x24);
				wr_comm(0xb0);
					 wr_auto(cur_index/10 + 16);
				wr_comm(0xb2);
			
				wr_xd((17),0x24);
				wr_comm(0xb0);
					 wr_auto(cur_index%10 + 16);
				wr_comm(0xb2);
		}
}

void initialize() 
{
   __disable_irq(); //disable all interrupts (while initializing)   

   clock_init();  //initialize sytem clock

   button_init();  //initialize buttons
	
	 lcd_init();

   SPI_init();   //initialize spi

   if(SD_init()){     // initialize sd card		 
	   draw_string(0,3, "error Initializing please restart with memory card properly inserted...\r\n"); // Display a string
		 while(1);
	}	 
	
	timer_init();	 
	
	__enable_irq();  //Now enable interrupts
}
