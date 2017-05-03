#include<x86.h>
#include<trap.h>
#include<stdio.h>
#include<picirq.h>


#define IO_TIMER1 0X040    // 8253 Timer #1 generates interruptes on IRQ-0.

/* *
 * Frequency of all three count-down timers; (TIMER_FREQ/freq)
 * is the appropriate count to generate a frequency of freq Hz.
 * */

#define TIMER_FREQ 1193182
#define TIMER_DIV(x)  ((TIMER_FREQ+(x)/2)/(x))

#define TIMER_MODE (IO_TIMER1+3)
#define TIMER_SEL0 0x00  // select counter 0
#define TIMER_RATEGEN 0x04  // mode 2, rate generator
/*工作方式2被称作速率波发生器。进入这种工作方式， OUTi输出高电平，装入计数值n后如果GATE为高电平，则立即开始计数，OUTi保持为高电平不变； 待计数值减到“1”和“0”之间， OUTi将输出宽度为一个CLKi周期的负脉冲，计数值为“0”时，自动重新装入计数初值n，实现循环计数，OUTi将输出一定频率的负脉冲序列， 其脉冲宽度固定为一个CLKi周期， 重复周期为CLKi周期的n倍。*/
#define TIMER_16BIT 0x30 // r/w counter 16 bits, LSB first 低位优先

volatile size_t ticks;

void clock_init(void)
{
	outb(TIMER_MODE,TIMER_SEL0|TIMER_RATEGEN|TIMER_16BIT);
	outb(IO_TIMER1,TIMER_DIV(100)%256);
	outb(IO_TIMER1,TIMER_DIV(100)/256);

	ticks=0;

	cprintf("++setup timer interrupts");
	pic_enable(IRQ_TIMER);
}
