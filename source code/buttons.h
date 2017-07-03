#ifndef BUTTONS_H
#define BUTTONS_H

#include <lpc214x.h>

void button_init(void);
__irq void next_button_ISR (void);
__irq void play_button_ISR (void);
__irq void power_button_ISR (void);

extern volatile unsigned char power, next_pending, slide_show;

#endif
