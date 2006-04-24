/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

static size_t lo_strnlen(const char * s, size_t count)
{
	const char *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

static int lo_vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	int len;
	unsigned long long num;
	int i,j,n;
	char *str, *end, c;
	const char *s;
	int flags;
	int field_width;
	int precision;
	int qualifier;	

	str = buf;
	end = buf + size - 1;

	if (end < buf - 1) {
		end = ((void *) -1);
		size = end - buf + 1;
	}

	for (; *fmt ; ++fmt) {
		if (*fmt != '%') {
			if (str <= end)
				*str = *fmt;
			++str;
			continue;
		}

		/* process flags */
		flags = 0;
		/* get field width */
		field_width = -1;
		/* get the precision */
		precision = -1;
		/* get the conversion qualifier */
		qualifier = 'l';

            ++fmt;
		/* default base */
		switch (*fmt) {
			case 'c':
				c = (unsigned char) va_arg(args, int);
				if (str <= end)
					*str = c;
				++str;
				while (--field_width > 0) {
					if (str <= end)
						*str = ' ';
					++str;
				}
				continue;

			case 's':
				s = va_arg(args, char *);
				if (!s)
					s = "<NULL>";

				len = lo_strnlen(s, precision);

				for (i = 0; i < len; ++i) {
					if (str <= end)
						*str = *s;
					++str; ++s;
				}
				while (len < field_width--) {
					if (str <= end)
						*str = ' ';
					++str;
				}
				continue;


			case '%':
				if (str <= end)
					*str = '%';
				++str;
				continue;

			case 'x':
				break;

			default:
				if (str <= end)
					*str = '%';
				++str;
				if (*fmt) {
					if (str <= end)
						*str = *fmt;
					++str;
				} else {
					--fmt;
				}
				continue;
		}
            num = va_arg(args, unsigned long);
            for (j=0,i=0;i<8 && str <= end;i++) {
                  if ( (n = ((unsigned long)(num & (0xf0000000ul>>(i*4)))) >> ((7-i)*4)) || j != 0) {
                        j = 1;
                        if (n >= 10)
                              n += 'a'-10;
                        else
                              n += '0';
                        *str = n;
                        ++str;
                  }
            }
            if (j == 0 && str <= end) {
                  *str = '0';
                  ++str;
            }
	}
	if (str <= end)
		*str = '\0';
	else if (size > 0)
		/* don't write out a null byte if the buf size is zero */
		*end = '\0';
	/* the trailing null byte doesn't count towards the total
	* ++str;
	*/
	return str-buf;
}

int DEBUG_sprintf(char *buf, size_t size, const char *fmt, ...) {
  va_list args;
  int printed_len;
  
  va_start(args, fmt);
  printed_len = lo_vsnprintf(buf, size, fmt, args);
  va_end(args);
  return printed_len;
}

int scan_uarts();
void DEBUG_puts( char *string );
int DEBUG_printf(const char *fmt, ...)
{
  va_list args;
  int printed_len;
  char printk_buf[1024+1];
  
  /* Emit the output into the temporary buffer */
  va_start(args, fmt);
  printed_len = lo_vsnprintf(printk_buf, sizeof(printk_buf)-1, fmt, args);
  printk_buf[printed_len] = 0;
  va_end(args);
  
  scan_uarts();
  /*DEBUG_puts(printk_buf);*/
  return printed_len;
}
