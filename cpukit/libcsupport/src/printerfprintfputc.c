/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/printer.h>
#include <rtems/bspIo.h>

#include <pthread.h>

static pthread_once_t fprintf_putc_once = PTHREAD_ONCE_INIT;

static FILE fprintf_putc_file;

static _READ_WRITE_RETURN_TYPE fprintf_putc_write(
  struct _reent            *ptr,
  void                     *cookie,
  char const               *buf,
  _READ_WRITE_BUFSIZE_TYPE  n
)
{
  _READ_WRITE_RETURN_TYPE i;
  _READ_WRITE_RETURN_TYPE m;

  m = (_READ_WRITE_RETURN_TYPE) n;
  for (i = 0; i < m; ++i) {
    rtems_putc(buf[i]);
  }

  return m;
}

static void fprintf_putc_init(void)
{
  FILE *f;

  f = &fprintf_putc_file;
  f->_flags = __SWR | __SNBF;
  f->_cookie = f;
  f->_write = fprintf_putc_write;
  __lock_init_recursive(f->_lock);
}

static int fprintf_putc_printer(void *context, const char *fmt, va_list ap)
{
  return vfprintf(context, fmt, ap);
}

void rtems_print_printer_fprintf_putc(rtems_printer *printer)
{
  pthread_once(&fprintf_putc_once, fprintf_putc_init);
  printer->context = &fprintf_putc_file;
  printer->printer = fprintf_putc_printer;
}
