#include<string.h>
#include<x86.h>

size_t strlen(const char *s) {
	size_t cnt = 0;
	while (*s++ != '\0') {
		cnt++;
	}
	return cnt;
}

size_t strnlen(const char *s, size_t len) {
	size_t cnt = 0;
	while (cnt < len && *s++ != '\0') {
		cnt++;
	}
	return cnt;
}

char *strcpy(char *dst, const char *src) {
#ifdef __HAVE_ARCH_STRCPY
	return __strcpy(dst, src);
#else
	char *p=dst;
	while((*p++=*src++)!='\0')
	continue;
	return dst;
#endif
}

char *strncpy(char *dst, const char *src, size_t len) {
	char *p = dst;
	while (len > 0) {
		if ((*p = *src) != '\0')
			src++;
		p++, len--;
	}
	return dst;
}

int strcmp(const char *s1, const char *s2) {
#ifdef __HAVE_ARCH_STRCMP
	return __strcmp(s1, s2);
#else
	while(*s1!='\0' && *s1==*s2)
	{
		s1++;
		s2++;
	}
	return (int)((unsigned char)*s1-(unsigned char)*s2);
#endif
}

int strncmp(const char *s1, const char *s2, size_t n) {
	while (n > 0 && *s1 != '\0' && *s1 == *s2) {
		s1++;
		s2++;
		n--;
	}
	return (n == 0) ? 0 : (int) ((unsigned char) *s1 - (unsigned char) *s2);
}

char *strchr(const char *s, char c) {
	while (*s != '\0') {
		if (*s == c) {
			return (char *) s;
		}
		s++;
	}
	return NULL;
}

char *strfind(const char *s, char c) {
	while (*s != '\0') {
		if (*s == c) {
			break;
		}
		s++;
	}
	return (char *) s;
}

long strtol(const char *s, char **endptr, int base) {
	int neg = 0;
	long val = 0;

	while (*s == ' ' || *s == '\t')
		s++;
	if (*s == '+')
		s++;
	else if (*s == '-') {
		s++;
		neg = 1;
	}
	if ((base == 0 || base == 16) && s[0] == '0' && s[1] == 'x') {
		s += 2, base = 16;
	} else if ((base == 0 || base == 8) && s[0] == '0') {
		s++, base = 8;
	} else if (base == 0)
		base = 10;

	while (1) {
		int dig;
		if (*s >= '0' && *s <= '9')
			dig = *s - '0';
		else if (*s >= 'a' && *s <= 'z')
			dig = *s - 'a' + 10;
		else if (*s >= 'A' && *s <= 'Z')
			dig = *s - 'A' + 10;
		else
			break;
		if(dig>=base)
			break;
		s++,val=val*base+dig;
	}
	if(endptr)
		*endptr=(char *)s;
	return (neg?-val:val);
}

void * memset(void *s,char c,size_t n){
#ifdef __HAVE_ARCH_MEMSET
	return __memset(s,c,n);
#else
	char *p=s;
	while(n-->0)
	{
		*p++=c;
	}
	return s;
#endif
}


void *memmove(void *dst, const void *src, size_t n){
#ifdef __HAVE_ARCH_MEMMOVE
	return __memmove(dst,src,n);
#else
	const char *s=src;
	char *d=dst;
	if(s<d && s+n>d){
		s+=n,d+=n;
		while(n-->0)
		{
			*--d=*--s;
		}
	}
	else{
		while(n-->0)
			*d++=*s++;
	}
	return dst;
#endif
}

void *memcpy(void *dst, const void *src, size_t n){
#ifdef __HAVE_ARCH_MEMCPY
	return __memcpy(dst,src,n);
#else
	const char*=src;
	char *d=dst;
	while(n-->0)
		*d++=*s++;
	return dst;
#endif
}



int memcmp(const void *v1, const void *v2, size_t n){
	const char *s1=(const char *)v1;
	const char *s2=(const char *)v2;
	while(n-->0){
		if(*s1!=*s2){
			return (int)((unsigned char)*s1 - (unsigned char)*s2);
		}
	}
	return 0;
}






















