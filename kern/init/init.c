#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <console.h>
#include <kdebug.h>
#include <picirq.h>
#include <trap.h>
#include <clock.h>
#include <intr.h>
#include <pmm.h>
#include <kmonitor.h>
#include<assert.h>

void kern_init(void) __attribute__((noreturn));
void grade_backtrace(void);
static void lab1_switch_test();

void kern_init() {
	extern char edata[], end[];
	memset(edata, 0, end - edata);
	cons_init();
	const char *message = "(THU.CST) os is loading ...";
	cprintf("%s\n\n", message);
	print_kerninfo();

	grade_backtrace();
	pmm_init();                 // init physical memory management,set TSS ,gdt
	pic_init();                 // init interrupt controller
	idt_init();                 // init interrupt descriptor table
	clock_init();               // init clock interrupt
	intr_enable();              // enable irq interrupt
	lab1_switch_test();
	while (1)
		continue;
}

void __attribute__((noinline))
grade_backtrace2(int arg0, int arg1, int arg2, int arg3) {
	mon_backtrace(0, NULL, NULL);
}

void __attribute__((noinline))
grade_backtrace1(int arg0, int arg1) {
	grade_backtrace2(arg0, (int) &arg0, arg1, (int) &arg1);
}

void __attribute__((noinline))
grade_backtrace0(int arg0, int arg1, int arg2) {
	grade_backtrace1(arg0, arg2);
}

void grade_backtrace(void) {
	grade_backtrace0(0, (int) kern_init, 0xffff0000);
}

static void lab1_print_cur_status(void) {
	static int round = 0;
	uint16_t reg1, reg2, reg3, reg4;
	asm volatile (
			"mov %%cs, %0;"
			"mov %%ds, %1;"
			"mov %%es, %2;"
			"mov %%ss, %3;"
			: "=m"(reg1), "=m"(reg2), "=m"(reg3), "=m"(reg4));
	cprintf("%d: @ring %d\n", round, reg1 & 3);
	cprintf("%d:  cs = %x\n", round, reg1);
	cprintf("%d:  ds = %x\n", round, reg2);
	cprintf("%d:  es = %x\n", round, reg3);
	cprintf("%d:  ss = %x\n", round, reg4);
	round++;
}

static void lab1_switch_to_user() {
	asm volatile(
			"sub $0x8,%%esp \n"
			"int %0 \n"
			"movl %%ebp,%%esp"
			:
			:"i"(T_SWITCH_TOU)
	);
}

static void lab1_switch_to_kernel() {
	asm volatile(
			"int %0 \n"
			"movl %%ebp,%%esp"
			:
			:"i"(T_SWITCH_TOK)
	);
}

static void lab1_switch_test(void) {
	lab1_print_cur_status();
	cprintf("+++ switch to  user  mode +++\n");
	lab1_switch_to_user();
	lab1_print_cur_status();
	cprintf("+++ switch to kernel mode +++\n");
	lab1_switch_to_kernel();
	lab1_print_cur_status();
}
