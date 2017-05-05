#include <defs.h>
#include <x86.h>
#include <picirq.h>

#define IO_PIC1 0X20 // Master (IRQs 0-7)
#define IO_PIC2 0XA0 // Slave (IRQs 8-15)

#define IRQ_SLAVE 2//IRQ at which slave connects to master

//级联，mask位置1为屏蔽,右边8位为主，高位（左）为从
// Current IRQ mask.
// Initial IRQ mask has interrupt 2 enabled (for slave 8259A).
static uint16_t irq_mask = 0xFFFF & ~(1 << IRQ_SLAVE);
static bool did_init = 0;

static void pic_setmask(uint16_t mask) {
	irq_mask = mask;
	if (did_init) {
		outb(IO_PIC1 + 1, mask);
		outb(IO_PIC2 + 1, mask >> 8);
	}
}

void pic_enable(unsigned int irq) {
	pic_setmask(irq_mask & ~(1 << irq)); //某位置0
}

void pic_init(void) {
	did_init = 1;

	outb(IO_PIC1 + 1, 0xff);
	outb(IO_PIC2 + 1, 0xff);

	// ICW1:  0001g0hi
	//    g:  0 = edge triggering, 1 = level triggering
	//    h:  0 = cascaded PICs, 1 = master only
	//    i:  0 = no ICW4, 1 = ICW4 required
	outb(IO_PIC1, 0X11);
	// ICW2:  Vector offset
	outb(IO_PIC1 + 1, IRQ_OFFSET);
	//ICW3:主片中的每一位表示相应的IR是否链接了从片
	outb(IO_PIC1 + 1, 1 << IRQ_SLAVE);
	// ICW4:  000nbmap
	//    n:  1 = special fully nested mode  当SFNM=0的时候，从片上的中断会使主片的ISR的bit 2被置位，同时屏蔽后续从片上的优先级更高的中断。当SFNM=1的时候当主片的ISR的bit 2被置位时不会屏蔽从片上优先级更高的中断。
	//    b:  1 = buffered mode
	//    m:  0 = slave PIC, 1 = master PIC
	//        (ignored when b is 0, as the master/slave role
	//         can be hardwired).
	//    a:  1 = Automatic EOI mode
	//    p:  0 = MCS-80/85 mode, 1 = intel x86 mode
	outb(IO_PIC1 + 1, 0x3);	//有可能被更低级的中断嵌套，autoEOI

	outb(IO_PIC2, 0X11);
	outb(IO_PIC2 + 1, IRQ_OFFSET + 8);
	outb(IO_PIC2 + 1, IRQ_SLAVE);
	outb(IO_PIC2 + 1, 0x3);             // ICW4

	outb(IO_PIC1, 0x68);    // clear specific mask
	outb(IO_PIC1, 0x0a);    // read IRR by default

	outb(IO_PIC2, 0x68);    // OCW3
	outb(IO_PIC2, 0x0a);    // OCW3

	if(irq_mask!=0xFFFF)
		pic_setmask(irq_mask);
}

