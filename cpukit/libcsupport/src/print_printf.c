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

int rtems_vprintf(
  const rtems_printer *printer,
  const char          *format,
  va_list              ap
)
{
  int len = 0;
  if ( rtems_print_printer_valid( printer ) ) {
    len = printer->printer( printer->context, format, ap );
  }
  return len;
}

int rtems_printf(
  const rtems_printer *printer,
  const char          *format,
  ...
)
{
  int len = 0;
  if ( rtems_print_printer_valid( printer ) ) {
    va_list ap;
    va_start( ap, format );
    len = printer->printer( printer->context, format, ap );
    va_end( ap );
  }
  return len;
}
