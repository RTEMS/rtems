/**
 *  @file
 *
 *  @brief Print Formatted Output
 *  @ingroup libcsupport
 */

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
  long long num,
  unsigned base,
  bool sign,
  unsigned maxwidth,
  char lead
);

/**
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
    enum {
      LFLAG_INT,
      LFLAG_LONG,
      LFLAG_LONG_LONG
    } lflag = LFLAG_INT;
    bool minus = false;
    bool sign = false;
    char lead = ' ';
    char c = *fmt;
    long long num;

    if (c != '%') {
      rtems_putc(c);
      continue;
    }

    ++fmt; c = *fmt;

    if (c == '0') {
      lead = '0';
      ++fmt; c = *fmt;
    }

    if (c == '-') {
      minus = true;
      ++fmt; c = *fmt;
    }

    while (c >= '0' && c <= '9' ) {
      width *= 10;
      width += ((unsigned) c - '0');
      ++fmt; c = *fmt;
    }

    if (c == 'l') {
      lflag = LFLAG_LONG;
      ++fmt; c = *fmt;

      if (c == 'l') {
        lflag = LFLAG_LONG_LONG;
        ++fmt; c = *fmt;
      }
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
      base = 16; sign = false; lflag = LFLAG_LONG;
    } else {
      rtems_putc(c);
      continue;
    }

    switch (lflag) {
      case LFLAG_INT:
        num = sign ? (long long) va_arg(ap, int)
          : (long long) va_arg(ap, unsigned int);
        break;
      case LFLAG_LONG:
        num = sign ? (long long) va_arg(ap, long)
          : (long long) va_arg(ap, unsigned long);
        break;
      case LFLAG_LONG_LONG:
        num = va_arg(ap, long long);
        break;
    }

    printNum(num, base, sign, width, lead);
  }
}

/**
 *  @brief Print Number in a Given Base
 *  @param[in] num is the number to print
 *  @param[in] base is the base used to print the number
 */
static void printNum(
  long long num,
  unsigned base,
  bool sign,
  unsigned maxwidth,
  char lead
)
{
  unsigned long long unsigned_num;
  unsigned long long n;
  unsigned count;
  #define UINT64_MAX_IN_OCTAL_FORMAT "1777777777777777777777"
  char toPrint[sizeof(UINT64_MAX_IN_OCTAL_FORMAT)];

  if ( sign && (num <  0) ) {
    rtems_putc('-');
    unsigned_num = (unsigned long long) -num;
    if (maxwidth) maxwidth--;
  } else {
    unsigned_num = (unsigned long long) num;
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
