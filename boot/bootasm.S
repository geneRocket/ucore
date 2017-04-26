#include<asm.h>

.set PROT_MODE


#目的转换到32位保护模式，进入C
#此时位于实模式，内存地址0x7c00,$cs=0 $ip=7c00


.set PROT_MODE_CSEG, 0x8  # kernel code segment selector
.set PROT_MODE_DSEG, 0x10 # kernel data segment selector,转换成10进制为16字节，对应第三个段描述符
.set CR0_PE_ON,0x1 # protected mode enable flag

#连接程序(ld)能够看到start
.globl start
.code16 #实模式16位
	cli #BIOS 作为一个小型操作系统，为了初始化硬件设备，可能设置了自己的中断处理程序。但是现在 BIOS 已经没有了控制权，而是引导加载器正在运行，所以现在还允许中断不合理也不安全
	cld

	xorw %ax,%ax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%ss

seta20.1:
	inb $0x64,%al
	testb $0x2,%al
	jnz seta20.1

	movb $0xd1,%al
	outb %al,$0x64


seta20.2:
	inb $0x64,%al
	testb $0x2,%al
	jnz seta20.2

	movb $0xdf,%al
	outb %al,$0x60

	ldgt gdtdesc
	movl %cr0, %eax
	orl $CR0_PE_ON, %eax
	movl %eax, %cr0

#ljmp的格式是： ljmp 段选择子，段内偏移
	ljmp $PROT_MODE_CSEG, $protcseg


.code32
protcseg:
	movw $PROT_MODE_DSEG,%ax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%fs
	movw %ax,%gs
	movw %ax,%ss


	movl $0x0,%ebp
	movl $start,%esp
	call bootmain

spin:
	jmp spin


.p2align 2
gdt:
	SEG_NULLASM
	SEG_ASM(STA_X|STA_R,0x0,0xffffffff)
	SEG_ASM(STA_W,0x0,0xffffffff)

gdtdesc:
	.word 0x17
	.long gdt
