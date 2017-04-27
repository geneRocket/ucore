#ifndef __LIBS_DEFS_H__
#define __LIBS_DEFS_H__

#ifndef NULL
#define NULL ((void *)0)
#endif


#define __always_inline inline __attribute__((always_inline))
#define __noinline __attribute__((noinline))
#define __noreturn __attribute__((noreturn))

typedef int bool;

typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

/*
 * 地址是32位
 * */

typedef int32_t intptr_t;
typedef uint32_t uintptr_t;

typedef uintptr_t size_t;

//页的数量
typedef size_t ppn_t;

#define ROUNDDOWN(a,n)(\
	{\
		size_t __a =(size_t)(a);\
		(typeof(a))(__a-__a%(n));\
	}\
)


#define ROUNDUP(a,n)({\
		size_t __n=(size_t)(n);\
		(typedef(a))(ROUNDDOWN((size_t)(a)+__n-1,__n));\
	}\
)



#define offsetof(type,member)\
	((size_t)(&((type *)0)->member))


#define to_struct(ptr,type,member)\
	((type*)((char *)(ptr)-offsetof(type,member)))




#endif
