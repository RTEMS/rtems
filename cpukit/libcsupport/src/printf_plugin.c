/**
 *  @file
 *
 *  @brief RTEMS Plugin Printf
 *  @ingroup libcsupport
 */

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
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/printer.h>

static int rtems_printf_plugin(void *context, const char *format, va_list ap)
{
  (void) context;
  return vprintf(format, ap);
}

void rtems_print_printer_printf(rtems_printer *printer)
{
  printer->context = NULL;
  printer->printer = rtems_printf_plugin;
}
