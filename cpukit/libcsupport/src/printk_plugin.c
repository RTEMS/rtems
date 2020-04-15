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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/printer.h>
#include <rtems/bspIo.h>

int rtems_printk_printer(
  void *ignored,
  const char *format,
  va_list ap
)
{
  (void) ignored;
  return vprintk( format, ap );
}

void rtems_print_printer_printk(
  rtems_printer *printer
)
{
  printer->context = NULL;
  printer->printer = rtems_printk_printer;
}
