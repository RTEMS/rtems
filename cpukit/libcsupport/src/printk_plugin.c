/**
 *  @file
 *
 *  @brief Plugin Printk
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include <rtems/print.h>

void rtems_print_printer_printk(
  rtems_printer *printer
)
{
  printer->context = NULL;
  printer->printer = printk_plugin;
}

int printk_plugin(
  void *ignored,
  const char *format,
  va_list ap
)
{
  (void) ignored;
  vprintk( format, ap );
  return 0;
}
