/*
	a safe version of printf that might be useful for debugging parts that
	are known to have problems e.g. with printf() e.t.c.
*/
#include <_ansi.h>
#ifdef _HAVE_STDC
#include <stdarg.h>
#else
#include <varargs.h>
#endif
char   kkBuf[1024];
/* Routine to do "console" in fully polled mode */
void static kkputs( const char *);
format_string(char * fmt, va_list ap, char * kkBuf);
#ifdef _HAVE_STDC
void
kkprintf (const char *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  format_string (fmt, *ap+4, kkBuf);
  kkputs(kkBuf);
  va_end (ap);
}
void mkvisable()
{
 kkputs("Hello");
}
#else
void
kkprintf(fmt, va_alist)
char * fmt;
va_dcl
{
	va_list ap;
	va_start(ap);
	format_string(fmt, ap, kkBuf);
	kkputs(kkBuf);
	va_end(ap);
}
#endif
extern int DBGConsole_make_sync;
void
kkputs( const char * buf)
{
	volatile unsigned int * consoleOP;
	unsigned char c;
	consoleOP = (unsigned int *) 0x1318;	/* Outbound Message 0 */
	while (( c = *buf++) != 0){
		while( DBGConsole_make_sync && (*consoleOP != 0))
			;
		*consoleOP = (unsigned int)c;
	}
}


/* we have got an error during build for 'isatty()' wo/ good reason
   we temporarily use this fix....
*/
isatty(int fd)
{
        return 1;
}

