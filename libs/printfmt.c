#include<defs.h>
#include<x86.h>
#include<error.h>
#include<stdio.h>
#include<string.h>

static const char * const error_string[MAXERROR + 1] = { [0]NULL, [E_UNSPECIFIED
		] "unspecified error", [E_BAD_PROC] "bad process", [E_INVAL
		] "invalid parameter", [E_NO_MEM] "out of memory", [E_NO_FREE_PROC
		] "out of processes", [E_FAULT] "segmentation fault", };
/* *
 * printnum - print a number (base <= 16) in reverse order
 * @putch:        specified putch function, print a single character
 * @putdat:        used by @putch function
 * @num:        the number will be printed
 * @base:        base for print, must be in [1, 16]
 * @width:         maximum number of digits, if the actual width is less than @width, use @padc instead
 * @padc:        character that padded on the left if the actual width is less than @width
 * */
static int printnum(void (*putch)(int, void *), void *putdat,
		unsigned long long num, unsigned base, int width, int padc) {
	unsigned long long result = num;
	unsigned mod = do_div(result, base);
	int len;
	if (num >= base) {
		len = printnum(putch, putdat, result, base, width - 1, padc) + 1;
	} else {
		if (padc != '-')
			while (--width > 0) {
				putch(padc, putdat);
			}
		len = 1;
	}
	putch("0123456789abcdef"[mod], putdat);
	return len;
}

/* *
 * getuint - get an unsigned int of various possible sizes from a varargs list
 * @ap:            a varargs list pointer
 * @lflag:        determines the size of the vararg that @ap points to
 * */
static unsigned long long getuint(va_list*ap, int lflag) {
	if (lflag >= 2)
		return va_arg(*ap, unsigned long long);
	else if (lflag)
		return va_arg(*ap, unsigned long);
	else
		return va_arg(*ap, unsigned int);
}

/* *
 * getint - same as getuint but signed, we can't use getuint because of sign extension
 * @ap:            a varargs list pointer
 * @lflag:        determines the size of the vararg that @ap points to
 * */
static long long getint(va_list *ap, int lflag) {
	if (lflag >= 2) {
		return va_arg(*ap, long long);
	} else if (lflag) {
		return va_arg(*ap, long);
	} else {
		return va_arg(*ap, int);
	}
}

/* *
 * printfmt - format a string and print it by using putch
 * @putch:        specified putch function, print a single character
 * @putdat:        used by @putch function
 * @fmt:        the format string to use
 * */

void printfmt(void (*putch)(int, void *), void * putdat, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vprintfmt(putch, putdat, fmt, ap);
	va_end(ap);
}

/* *
 * vprintfmt - format a string and print it by using putch, it's called with a va_list
 * instead of a variable number of arguments
 * @putch:        specified putch function, print a single character
 * @putdat:        used by @putch function
 * @fmt:        the format string to use
 * @ap:            arguments for the format string
 *
 * Call this function if you are already dealing with a va_list.
 * Or you probably want printfmt() instead.
 * */

void vprintfmt(void (*putch)(int, void *), void *putdat, const char *fmt,
		va_list ap) {
	register const char *p;
	register int ch, err;
	unsigned long long num;
	int base, width, precision, lflag, altflag;

	while (1) {
		while ((ch = *(unsigned char*) fmt++) != '%') {
			if (ch == '\0')
				return;
			putch(ch, putdat);
		}
		char padc = ' ';
		width = precision = -1;
		lflag = altflag = 0;

		reswitch: switch (ch = *(unsigned char *) fmt++) {
		// flag to pad on the right
		case '-':
			padc = '-';
			goto reswitch;
			// flag to pad with 0's instead of spaces
		case '0':
			padc = '0';
			goto reswitch;

			//width field
		case '1' ... '9':
			for (precision = 0;; ++fmt) {
				//ch为 fmt 的前一个字符
				precision = precision * 10 + ch - '0';

				ch = *fmt;
				if (ch < '0' || ch > '9')
					break;
			}
			goto process_precision;

			//用参数代替格式数字
		case '*':
			precision = va_arg(ap, int);
			goto process_precision;
		case '.':
			if (width < 0)
				width = 0;
			goto reswitch;
			//8，16进制加前缀
		case '#':
			altflag = 1;
			goto reswitch;

			process_precision:
			//width<0表示还没有处理宽度
			if (width < 0)
				width = precision, precision = -1;
			goto reswitch;
		case 'l':
			lflag++;
			goto reswitch;

		case 'c':
			putch(va_arg(ap, int), putdat);
			break;

		case 'e':
			err = va_arg(ap, int);
			if (err < 0)
				err = -err;
			if (err > MAXERROR || (p = error_string[err]) == NULL) {
				printfmt(putch, putdat, "error %d", err);
			} else
				printfmt(putch, putdat, "%s", p);
			break;
		case 's':
			if ((p = va_arg(ap, char *)) == NULL) {
				p = "(null)";
			}
			if (width > 0 && padc != '-') {
				//strnlen size_t 无符号，-1 无符号最大，precision默认-1
				for (width -= strnlen(p, precision); width > 0; width--) {

					putch(padc, putdat);
				}
			}
			for (; (ch = *p++) != '\0' && (precision < 0 || --precision >= 0);
					width--) {
				if (altflag && (ch < ' ' || ch > '~'))
					putch('?', putdat);
				else
					putch(ch, putdat);
			}
			for (; width > 0; width--) {
				putch(' ', putdat);
			}
			break;
		case 'd':
			num = getint(&ap, lflag);
			if ((long long) num < 0) {
				putch('-', putdat);
				num = -(long long) num;
			}
			base = 10;
			goto number;
		case 'u':
			num = getuint(&ap, lflag);
			base = 10;
			goto number;
		case 'o':
			num = getuint(&ap, lflag);
			base = 8;
			goto number;
			//pointer
		case 'p':
			putch('0', putdat);
			putch('x', putdat);
			num = (unsigned long long) (uintptr_t) va_arg(ap, void *);
			base = 16;
			goto number;
		case 'x':
			num = getuint(&ap, lflag);
			base = 16;

			number: {

				int numlen = printnum(putch, putdat, num, base, width, padc);
				if (padc == '-')
					while (numlen++ < width)
						putch(' ', putdat);
			}
			break;

		case '%':
			putch(ch, putdat);
			break;
		default:
			putch('%', putdat);
			for (fmt--; fmt[-1] != '%'; fmt--)
				continue;
			break;
		}

	}
}

/* sprintbuf is used to save enough information of a buffer */
struct sprintbuf {
	char *buf; // address pointer points to the first unused memory
	char *ebuf;            // points the end of the buffer
	int cnt;    // the number of characters that have been placed in this buffer
};

/* *
 * sprintputch - 'print' a single character in a buffer
 * @ch:            the character will be printed
 * @b:            the buffer to place the character @ch
 * */

static void sprintputch(int ch, struct sprintbuf *b) {
	b->cnt++;
	if (b->buf < b->ebuf)
		*b->buf++ = ch;
}

int snprintf(char *str, size_t size, const char *fmt, ...) {
	va_list ap;
	int cnt;
	va_start(ap, fmt);
	cnt = vsnprintf(str, size, fmt, ap);
	va_end(ap);
	return cnt;
}

int vsnprintf(char *str, size_t size, const char *fmt, va_list ap) {
	struct sprintbuf b = { str, str + size - 1, 0 };
	if (str == NULL || b.buf > b.ebuf) {
		return -E_INVAL;
	}
	vprintfmt((void*) sprintputch, &b, fmt, ap);
	*b.buf = '\0';
	return b.cnt;
}

