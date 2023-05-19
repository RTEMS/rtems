/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/shell.h>
#include <rtems/shellconfig.h>
#include <rtems/cpuuse.h>
#include <rtems/printer.h>

static int rtems_shell_main_cpuinfo(int argc, char **argv)
{
  rtems_printer printer;

  rtems_print_printer_fprintf(&printer, stdout);
  rtems_cpu_info_report(&printer);

  return 0;
}

rtems_shell_cmd_t rtems_shell_CPUINFO_Command = {
  .name = "cpuinfo",
  .usage = "cpuinfo",
  .topic = "rtems",
  .command = rtems_shell_main_cpuinfo
};
