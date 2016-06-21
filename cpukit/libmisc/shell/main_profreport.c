/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <stdio.h>

#include <rtems/profiling.h>
#include <rtems/printer.h>
#include <rtems/shell.h>
#include <rtems/shellconfig.h>

static int rtems_shell_main_profreport(int argc, char **argv)
{
  rtems_printer printer;
  rtems_print_printer_printf(&printer);
  rtems_profiling_report_xml(
    "Shell",
    &printer,
    0,
    "  "
  );

  return 0;
}

rtems_shell_cmd_t rtems_shell_PROFREPORT_Command = {
  .name = "profreport",
  .usage = "profreport",
  .topic = "rtems",
  .command = rtems_shell_main_profreport
};
