#ifndef LCD_H
#define LCD_H

#include <lpc214x.h>
#include <string.h>

#define power (1<<21)
#define fs  (1<<24)
#define cd  (1<<28)
#define ce  (1<<29)
#define rd  (1<<26)
#define wr  (1<<27)
#define rst (1<<25)
#define bf0  (1<<16)
#define bf1  (1<<17)
#define bf3  (1<<19)


#define width      16       //Display width
#define addr_w     0x0000   //Text display buffer base address
#define addr_t     0x0100   //Graphics display buffer base address

void lcd_init (void);
void wr_od (unsigned char data,unsigned char command);
void wr_td (unsigned char data_l,unsigned char data_h,unsigned char command);
void wr_xd (unsigned short data,unsigned short command);
void wr_auto (unsigned char data);
void wr_comm (unsigned char command);
void wr_data (unsigned char data);
void chk_busy (unsigned char autowr);
void clrram (void);
void disp_img (unsigned char addr,unsigned char xl,unsigned char yl,unsigned char const *img);
void draw_string(unsigned int x, unsigned int y, char *str);

void delay(unsigned int limit);

#endif
