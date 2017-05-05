#include<memlayout.h>
# vectors.S sends all traps here.
.text
.globl __alltraps
__alltraps:
	# push registers to build a trap frame
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
	pushal

	movl $GD_KDATA, %eax
    movw %ax, %ds
    movw %ax, %es


    # push %esp to pass a pointer to the trapframe as an argument to trap()
    pushl %esp
	call trap

	popl %esp

.globl __trapret

__trapret:
	popal
	popl %gs
    popl %fs
    popl %es
    popl %ds

    # get rid of the trap number and error code
    addl $0x8,%esp
    iret
