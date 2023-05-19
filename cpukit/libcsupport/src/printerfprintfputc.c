/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
