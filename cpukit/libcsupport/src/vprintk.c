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
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include <stdio.h>
#include <rtems/bspIo.h>

static void printNum(
  long unsigned int num,
  int               base,
  int               sign,
  int               maxwidth,
  int               lead
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
  char     c, *str;
  int      lflag, base, sign, width, lead, minus;

  for (; *fmt != '\0'; fmt++) {
    lflag = 0;
    base  = 0;
    sign = 0;
    width = 0;
    minus = 0;
    lead = ' ';
    if (*fmt != '%') {
      BSP_output_char(*fmt);
      continue;
    }
    fmt++;
    if (*fmt == '0' ) {
      lead = '0';
      fmt++;
    }
    if (*fmt == '-' ) {
      minus = 1;
      fmt++;
    }
    while (*fmt >= '0' && *fmt <= '9' ) {
      width *= 10;
      width += (*fmt - '0');
      fmt++;
    }

    if ((c = *fmt) == 'l') {
      lflag = 1;
      c = *++fmt;
    }
    if ( c == 'c' ) {
      BSP_output_char(va_arg(ap, int));
      continue;
    }
    if ( c == 's' ) {
      int i, len;
      char *s;

      str = va_arg(ap, char *);

      /* calculate length of string */
      for ( len=0, s=str ; *s ; len++, s++ )
        ;

      /* leading spaces */
      if ( !minus )
        for ( i=len ; i<width ; i++ )
          BSP_output_char(' ');

      /* no width option */
      if (width == 0) {
          width = len;
      }

      /* output the string */
      for ( i=0 ; i<width && *str ; str++ )
        BSP_output_char(*str);

      /* trailing spaces */
      if ( minus )
        for ( i=len ; i<width ; i++ )
          BSP_output_char(' ');

      continue;
    }

    /* must be a numeric format or something unsupported */
    if ( c == 'o' || c == 'O' ) {
      base = 8; sign = 0;
    } else if ( c == 'i' || c == 'I' ||
                c == 'd' || c == 'D' ) {
      base = 10; sign = 1;
    } else if ( c == 'u' || c == 'U' ) {
      base = 10; sign = 0;
    } else if ( c == 'x' || c == 'X' ) {
      base = 16; sign = 0;
    } else if ( c == 'p' ) {
      base = 16; sign = 0; lflag = 1;
    } else {
      BSP_output_char(c);
      continue;
    }

    printNum(
      lflag ? va_arg(ap, long int) : (long int)va_arg(ap, int),
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
  long unsigned int num,
  int               base,
  int               sign,
  int               maxwidth,
  int               lead
)
{
  long unsigned int n;
  int               count;
  char              toPrint[20];

  if ( (sign == 1) && ((long)num <  0) ) {
    BSP_output_char('-');
    num = -num;
    if (maxwidth) maxwidth--;
  }

  count = 0;
  while ((n = num / base) > 0) {
    toPrint[count++] = (num - (n*base));
    num = n;
  }
  toPrint[count++] = num;

  for (n=maxwidth ; n > count; n-- )
    BSP_output_char(lead);

  for (n = 0; n < count; n++) {
    BSP_output_char("0123456789ABCDEF"[(int)(toPrint[count-(n+1)])]);
  }
}
