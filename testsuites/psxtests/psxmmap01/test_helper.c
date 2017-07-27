/*
 * Copyright (c) 2017 Kevin Kirspel.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "test_helper.h"

#include <stdio.h>
#include <stdarg.h>

void fail_check(
  const char *file,
  const size_t line,
  const char *fmt,
  ...
)
{
    va_list ap;

    printf( "%s: %zd: ", file, line );

    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}
