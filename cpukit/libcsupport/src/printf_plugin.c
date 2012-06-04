/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/bspIo.h>

#include <stdio.h>

int rtems_printf_plugin(void *context, const char *format, ...)
{
  int rv;
  va_list ap;

  va_start(ap, format);
  rv = vprintf(format, ap);
  va_end(ap);

  return rv;
}
