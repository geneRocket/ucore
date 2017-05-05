#include<defs.h>
#include<stdio.h>
#include<console.h>

static void cputch(int c,int *cnt)
{
	cons_putc(c);
	(*cnt)++;
}

int vcprintf(const char *fmt,va_list ap)
{
	int cnt=0;
	vprintfmt((void *)cputch,&cnt,fmt,ap);
	return cnt;
}

int cprintf(const char *fmt,...){
	va_list ap;
	int cnt;
	va_start(ap,fmt);
	cnt=vcprintf(fmt,ap);
	va_end(ap);
	return cnt;
}


void cputchar(int c){
	cons_putc(c);
}

int cputs(const char *str){
	int cnt=0;
	char c;
	while((c=*str++)!='\0'){
		cputch(c,&cnt);
	}
	cputch('\n',&cnt);
	return cnt;
}

int getchar()
{
	int c;
	while((c=cons_getc())==0)
		continue;
	return c;
}
