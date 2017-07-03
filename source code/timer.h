#ifndef TIMER_H
#define TIMER_H

#define MR0I (1<<0) //Interrupt When TC matches MR0
#define MR0R (1<<1)	//Reset TC when TC matches MR0

#define DELAY_MS 5000 //5 Second(s) Delay
#define PRESCALE 48000 //48000 PCLK clock cycles to increment TC by 1 

void timer_init(void);
__irq void T0ISR(void);

#endif
