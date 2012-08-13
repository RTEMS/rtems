/*
 * (C) Copyright 1997 -
 * - NavIST Group - Real-Time Distributed Systems and Industrial Automation
 *
 * http://pandora.ist.utl.pt
 *
 * Instituto Superior Tecnico * Lisboa * PORTUGAL
 *
 * Disclaimer:
 *
 * This file is provided "AS IS" without warranty of any kind, either
 * expressed or implied.
 *
 * This code is based on code by: Jose Rufino - IST
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <rtems/bspIo.h>

static void printNum(
  long num,
  unsigned base,
  bool sign,
  unsigned maxwidth,
  char lead
);

/*
 *  vprintk
 *
 *  A simplified version of printf intended for use when the
 *  console is not yet initialized or in ISR's.
 *
 * Arguments:
 *    as in printf: fmt - format string, ... - unnamed arguments.
 */
void vprintk(
  const char *fmt,
  va_list     ap
)
{
  for (; *fmt != '\0'; fmt++) {
    unsigned base = 0;
    unsigned width = 0;
    bool lflag = false;
    bool minus = false;
    bool sign = false;
    char lead = ' ';
    char c;

    if (*fmt != '%') {
      rtems_putc(*fmt);
      continue;
    }
    fmt++;
    if (*fmt == '0' ) {
      lead = '0';
      fmt++;
    }
    if (*fmt == '-' ) {
      minus = true;
      fmt++;
    }
    while (*fmt >= '0' && *fmt <= '9' ) {
      width *= 10;
      width += ((unsigned) *fmt - '0');
      fmt++;
    }

    if ((c = *fmt) == 'l') {
      lflag = true;
      c = *++fmt;
    }
    if ( c == 'c' ) {
      /* need a cast here since va_arg() only takes fully promoted types */
      char chr = (char) va_arg(ap, int);
      rtems_putc(chr);
      continue;
    }
    if ( c == 's' ) {
      unsigned i, len;
      char *s, *str;

      str = va_arg(ap, char *);

      if ( str == NULL ) {
        str = "";
      }

      /* calculate length of string */
      for ( len=0, s=str ; *s ; len++, s++ )
        ;

      /* leading spaces */
      if ( !minus )
        for ( i=len ; i<width ; i++ )
          rtems_putc(' ');

      /* no width option */
      if (width == 0) {
          width = len;
      }

      /* output the string */
      for ( i=0 ; i<width && *str ; str++ )
        rtems_putc(*str);

      /* trailing spaces */
      if ( minus )
        for ( i=len ; i<width ; i++ )
          rtems_putc(' ');

      continue;
    }

    /* must be a numeric format or something unsupported */
    if ( c == 'o' || c == 'O' ) {
      base = 8; sign = false;
    } else if ( c == 'i' || c == 'I' ||
                c == 'd' || c == 'D' ) {
      base = 10; sign = true;
    } else if ( c == 'u' || c == 'U' ) {
      base = 10; sign = false;
    } else if ( c == 'x' || c == 'X' ) {
      base = 16; sign = false;
    } else if ( c == 'p' ) {
      base = 16; sign = false; lflag = true;
    } else {
      rtems_putc(c);
      continue;
    }

    printNum(
      lflag ? va_arg(ap, long) : (long) va_arg(ap, int),
      base,
      sign,
      width,
      lead
    );
  }
}

/*
 * printNum - print number in a given base.
 * Arguments
 *    num - number to print
 *    base - base used to print the number.
 */
static void printNum(
  long num,
  unsigned base,
  bool sign,
  unsigned maxwidth,
  char lead
)
{
  unsigned long unsigned_num;
  unsigned long n;
  unsigned count;
  char toPrint[20];

  if ( sign && (num <  0) ) {
    rtems_putc('-');
    unsigned_num = (unsigned long) -num;
    if (maxwidth) maxwidth--;
  } else {
    unsigned_num = (unsigned long) num;
  }

  count = 0;
  while ((n = unsigned_num / base) > 0) {
    toPrint[count++] = (char) (unsigned_num - (n * base));
    unsigned_num = n;
  }
  toPrint[count++] = (char) unsigned_num;

  for (n=maxwidth ; n > count; n-- )
    rtems_putc(lead);

  for (n = 0; n < count; n++) {
    rtems_putc("0123456789ABCDEF"[(int)(toPrint[count-(n+1)])]);
  }
}
