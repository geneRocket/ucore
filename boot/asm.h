#ifndef __BOOT_ASM_H__
#define __BOOT_ASM_H__


/*word 16位*/

/*x86段描述符*/
#define SEG_NULLASM \
    .word 0,0;      \
    .byte 0,0,0,0

/*
 * f 4个1
 * limit 颗粒度G In units of 4 Kilobytes, to define a limit of up to 4 gigabytes. The limit is shifted left by 12 bits when loaded, and low-order one-bits are inserted.
 *code and data segments
 */

#define SEG_ASM(type,base,lim) \
	.word (((lim)>>12) & 0xffff),((base)&0xffff);\
	.byte(((base)>>16)&0xff) ,(0x90|(type)),\
	(0xC0 | (((lim) >> 28) & 0xf)),(((base)>>24)&0xff)







/*type*/
#define STA_X       0x8     // Executable segment
#define STA_E       0x4     // Expand down (non-executable segments)
#define STA_C       0x4     // Conforming code segment (executable only)
#define STA_W       0x2     // Writeable (non-executable segments)
#define STA_R       0x2     // Readable (executable segments)
#define STA_A       0x1     // Accessed

#endif
