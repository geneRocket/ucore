#ifndef __LIBS_X86_H__
#define __LIBS_X86_H__

#include<defs.h>

/*
 * 避免溢出的64位除法，先用高位部分除，得到的余数组成下一次的高位再继续除，商用两次的合并在一起
 * 拆成2个32位，求出高位部分的商和余数
 * 然后在余数和低位作为被除数，进行64位除法
 * 合成第一次的商和第二次的商
 */

#define do_div(n,base)({\
	unsigned long __upper,__low,__high,__mod,__base;\
	__base=(base);\
	asm("":"=a"(__low),"=d"(__high):"A"(n));\
	__upper=__high;\
	if(__high!=0)\
	{\
		__upper=__high%__base;\
		__high=__high/__base;\
	}\
	asm("divl %2": "=a"(__low),"=d"(__mod)      : "rm"(__base),"0"(__low) ,"1"(__upper));       \
	asm(" ": "=A"(n)  :"a"(__low),"d"(__high));\
	__mod;\
})

static inline uint8_t inb(uint16_t port) __attribute__((always_inline));
static inline void insl(uint32_t port, void *addr, int cnt)
		__attribute__((always_inline)); //从端口读4字节
static inline void outb(uint16_t port, uint8_t data)
		__attribute__((always_inline));
static inline void outw(uint16_t port, uint16_t data)
		__attribute__((always_inline));
static inline uint32_t read_ebp() __attribute__((always_inline));

struct pseudodesc {
	uint16_t pd_lim; //Limit
	uint32_t pd_base; //base address
}__attribute__ ((packed));
//取消结构在编译过程中的优化对齐,按照实际占用字节数进行对齐

static inline void lidt(struct pseudodesc * pd) __attribute__((always_inline));
static inline void sti(void) __attribute__((always_inline)); //允许硬件中断
static inline void cli(void) __attribute__((always_inline)); //关闭硬件中断
static inline void ltr(uint16_t sel) __attribute__((always_inline)); //LTR指令是专门用于装载任务状态段寄存器TR的指令。该指令的操作数是对应TSS段描述符的选择子。LTR指令从GDT中取出相应的TSS段描述符，把TSS段描述符的基地址和界限等信息装入TR的高速缓冲寄存器中。

static inline uint8_t inb(uint16_t port) {
	uint8_t data;
	asm volatile("inb %1,%0": "=a"(data) : "d"(port) );
	return data;
}

static inline void insl(uint32_t port, void *addr, int cnt) {
	asm volatile(
			"cld;"
			"repne;insl;"
			:"=D"(addr),"=c"(cnt)
			:"d"(port),"0"(addr),"1"(cnt)
			:"memory","cc"
	);
	//"memory",修改了内存
	//"cc",修改了标志寄存器

}

static inline void outb(uint16_t port, uint8_t data) {
	asm volatile(
			"outb %0,%1"
			:
			:"a"(data),"d"(port)
	);
}

static inline void outw(uint16_t port, uint16_t data) {
	asm volatile(
			"outw %0,%1"
			:
			:"a"(data),"d"(port)
	);
}

static inline uint32_t read_ebp(void) {
	uint32_t ebp;
	asm volatile("movl %%ebp,%0":"=r"(ebp));
	return ebp;
}

static inline void lidt(struct pseudodesc *pd) {
	asm volatile("lidt (%0)"::"r"(pd));
}

static inline void sti(void) {
	asm volatile ("sti");
}

static inline void cli() {
	asm volatile ("cli");
}

static inline void ltr(uint16_t sel) {
	asm volatile("ltr %0"::"r"(sel));
}

static inline int __strcmp(const char *s1, const char *s2)
		__attribute__((always_inline));
static inline char * __strcpy(char *dst, const char *src)
		__attribute__((always_inline));
static inline void * __memset(void *s, char c, size_t n)
		__attribute__((always_inline));
static inline void * __memmove(void *dst, const void * src, size_t n)
		__attribute__((always_inline));
static inline void *__memcpy(void *dst, const void *src, size_t n) __attribute__((always_inline));


#ifndef __HAVE_ARCH_STRCMP
#define __HAVE_ARCH_STRCMP

static inline int
__strcmp(const char *s1,const char *s2){
	int d0, d1,ret;
	asm volatile(
			"1: lodsb;"
			"scasb;"
			"jne 2f;"
			"testb %%al,%%al;"//相与，为0的时候（字符串结束）为0
			"jne 1b;"
			"xorl %%eax,%%eax;"
			"jmp 3f;"
			"2: sbbl %%eax,%%eax;"
			"orb $1,%%al;"//s1>s2大于0的,上面得出0,或1
			"3:"
			:"=a"(ret),"=&S"(d0),"=&D"(d1)
			:"1"(s1),"2"(s2)
			:"memory"
	);
	return ret;
}
/*用符号&进行修饰时,等于向GCC声明:"GCC不得为任何Input操作表达式分配与此Output操作表达式相同的寄存器";*/
//lodsb其具体操作是把SI指向的存储单元读入累加器,其中LODSB是读入AL.然后SI自动增加或减小1或2位.当方向标志位DF=0时，则SI自动增加；DF=1时，SI自动减小。
//scasb 计算 AL - byte of [ES:EDI] , 设置相应的标志寄存器的值；修改寄存器EDI的值：如果标志DF为0，则 inc EDI；如果DF为1，则 dec EDI。
#endif /* __HAVE_ARCH_STRCMP */


#ifndef __HAVE_ARCH_STRCPY
#define __HAVE_ARCH_STRCPY

static inline char * __strcpy(char *dst,const char *src){
	int d0,d1,d2;
	asm volatile(
			"1: lodsb;"
			"stosb;"
			"test %%al,%%al;"
			"jne 1b;"
			:"=&S"(d0),"=&D"(d1),"=&a"(d2)
			:"0"(src),"1"(dst)
			:"memory"
	);
	return dst;
}

#endif //__HAVE_ARCH_STRCPY

#ifndef __HAVE_ARCH_MEMSET
#define __HAVE_ARCH_MEMSET
static inline void * __memset(void *s,char c,size_t n){
	int d0,d1;
	asm volatile(
			"rep;"
			"stosb"
			:"=&c"(d0),"=&D"(d1)
			:"0"(n),"1"(s),"a"(c)
			:"memory"
	);
	return s;
}
#endif //__HAVE_ARCH_MEMSET


#ifndef __HAVE_ARCH_MEMMOVE
#define __HAVE_ARCH_MEMMOVE
static inline void * __memmove(void *dst,const void *src,size_t n)
{
	if(dst<src){
		return __memcpy(dst,src,n);
	}
	int d0,d1,d2;
	asm volatile(
			"std;"
			"rep;movsb;"
			"cld;"
			:"=&c"(d0),"=&S"(d1),"=&D"(d2)
			:"0"(n),"1"(src+n-1),"2"(dst+n-1)
			:"memory"
	);
	return dst;
}

#endif //__HAVE_ARCH_MEMMOVE

#ifndef ____HAVE_ARCH_MEMCPY
#define __HAVE_ARCH_MEMCPY
static inline void * __memcpy(void * dst,const void *src,size_t n){
	int d0,d1,d2;
	asm volatile(
			"rep;movsl;"
			"movl %4,%%ecx;"
			"andl $3,%%ecx;"
			"jz 1f;"
			"rep;movsb;"
			"1:"
			:"=&c"(d0),"=&D"(d1),"=&S"(d2)
			:"0"(n/4),"g"(n),"1"(dst),"2"(src)
			:"memory"
	);
	return dst;
}



#endif //__HAVE_ARCH_MEMCPY







#endif
