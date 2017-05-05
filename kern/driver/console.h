#ifndef __KERN_DRIVER_CONSOLE_H__
#define __KERN_DRIVER_CONSOLE_H__

void cons_init();
void cons_putc(int c);
int cons_getc(void);
void serial_intr(void);
void kbd_intr();


#endif
