/*
 * Copyright (c) 2017 Chris Johns <chrisj@rtems.org>. All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/test.h>

int __wrap_printf(const char* format, ...);
int __wrap_puts(const char *str);
int __wrap_putchar(int c);

int __wrap_printf(
 const char* format,
  ...
)
{
  va_list ap;
  int len;
  va_start(ap, format);
  len = rtems_vprintf(
    &rtems_test_printer,
    format,
    ap
  );
  va_end(ap);
  return len;
}

int __wrap_puts(
  const char *str
)
{
  return rtems_test_printf( "%s\n", str );
}

int __wrap_putchar(
  int c
)
{
  return rtems_test_printf( "%c", c );
}
