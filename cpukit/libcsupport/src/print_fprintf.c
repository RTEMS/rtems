/**
 *  @file
 *
 *  @brief RTEMS Print Support
 *  @ingroup libcsupport
 */

/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/printer.h>

static int rtems_fprintf_plugin(void *context, const char *fmt, va_list ap)
{
  return vfprintf(context, fmt, ap);
}

void rtems_print_printer_fprintf(rtems_printer *printer, FILE *file)
{
  printer->context = file;
  printer->printer = rtems_fprintf_plugin;
}
