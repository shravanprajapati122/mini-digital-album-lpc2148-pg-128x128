/********************************************************************************************************
The lcd interface here was written based on the code by Kruck Wang(Luo XieBing)
from Labcenter Electronics Ltd. on 2009-04-23
********************************************************************************************************/
#include "lcd.h"

void lcd_init (void)
{    
	 IO0DIR |= power;
	 IO0SET |= power;
   delay(20000);
	
   IO1DIR |=  0x3fff0000; 
   IO1SET |= rst;
   delay(20000);
   IO1CLR |= ce;
   IO1SET |= wr;
   IO1SET |= rd;
   wr_xd(addr_w,0x40);                   // Set text display buffer base address
   wr_xd(addr_t,0x42);                   // Set graphics display buffer base address
   wr_td(width,0x00,0x41);               // Set text display width
   wr_td(width,0x00,0x43);               // Set graphics display width
   wr_comm(0x81);                        // Set display mode: text xor graphics
   wr_td(0x56,0x00,0x22);                // Set CGRAM offset address
   wr_comm(0x9c);                        // Set text and graphic display on
}
/*------------Write data or command to LCD--------------*/
void wr_od (unsigned char data,unsigned char command)       //write one byte data and one byte command
{
   wr_data(data);
   wr_comm(command);
}

void wr_td (unsigned char data_l,unsigned char data_h,unsigned char command)  //write two bytes data and one byte command
{ 
   wr_data(data_l);
   wr_data(data_h);
   wr_comm(command);
}

void wr_xd (unsigned short data,unsigned short command)       //write a 16 bits data and one byte command
{ 
   wr_data(data);
   wr_data(data>>8);
   wr_comm(command);
}

void wr_auto (unsigned char data)               //Auto write
 { 
   chk_busy (1);
   IO1CLR |= cd;
   IO1SET |= rd;
   IO1PIN=(IO1PIN&0XFF00FFFF)|(data<<16);
   IO1CLR |= wr;
   IO1SET |= wr;
 }

void wr_comm (unsigned char command)       //write command
 { 
   chk_busy (0);
   IO1SET |= cd;
   IO1SET |= rd;
   IO1PIN=(IO1PIN&0XFF00FFFF)|(command<<16);
   IO1CLR |= wr;
   IO1SET |= wr;
 }

void wr_data (unsigned char data)       //write data
 { 
   chk_busy (0);
   IO1CLR |= cd;
   IO1SET |= rd;
   IO1PIN=(IO1PIN&0XFF00FFFF)|(data<<16);
   IO1CLR |= wr;
   IO1SET |= wr;
 }

void chk_busy (unsigned char autowr)    //test busy flag
{ 
   IO1SET |= 0x00FF0000;
   IO1SET |= cd;
   IO1SET |= wr;
   IO1DIR &= 0xFF00FFFF;
   IO1CLR |= rd;
   if(autowr)
    { while((IO1PIN&bf3)==0);
    }
   else
    { while((IO1PIN&bf0)==0||(IO1PIN&bf1)==0);
    }
   IO1SET |= rd;
   IO1DIR |= 0x00FF0000;
}

/*--------------Clear RAM------------------*/
void clrram (void)
{ 
   unsigned char i,j;
   wr_xd(addr_w,0x24);
   wr_comm(0xb0);
   for(j=0;j<144;j++)
    { 
     for(i=0;i<width;i++)
         wr_auto(0x00);
    }
   wr_comm(0xb2);
}

/*--------------Draw a (8*l)*yl picture at addr-------------*/
void disp_img (unsigned char addr,unsigned char xl,unsigned char yl,unsigned char const *img)
{ 
    unsigned char i,j;
    for(j=0;j<yl;j++)
    { 
       for(i=0;i<xl;i++)
       { 
         wr_xd(addr+0X100+j*width+i,0x24);
         wr_od(img[j*xl+i],0xc0);
       }
    }
}

/*----------Draw a string at position (x,y)----------*/
void draw_string(unsigned int x,unsigned int y, char *str)
{ 
   char c;
   wr_xd((addr_w+16*y+x),0x24);
   wr_comm(0xb0);
   while(*str!='\0')
   { 
      c = (*str);
      wr_auto(c-32);
      str++;
   }
   wr_comm(0xb2);
}

void delay(unsigned int limit){
int m;  
      for(m=0; m<limit; m++);
}
