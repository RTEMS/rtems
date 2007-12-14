/*
 *  CPUUSE Command Implementation
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include <rtems.h>
#include <rtems/cpuuse.h>
#include <rtems/shell.h>
#include "internal.h"

int main_cpuuse(int argc,char *argv[])
{
  if ( argc >= 1 && !strcmp( argv[1], "-r" ) ) {
    printf( "Resetting CPU Usage information\n" );
    rtems_cpu_usage_reset();
  } else
    rtems_cpu_usage_report_with_plugin(stdout, (rtems_printk_plugin_t)fprintf);
  return 0;
}

shell_cmd_t Shell_CPUUSE_Command = {
  "cpuuse",                                   /* name */
  "[-r] print or reset per thread cpu usage", /* usage */
  "rtems",                                    /* topic */
  main_cpuuse,                                /* command */
  NULL,                                       /* alias */
  NULL                                        /* next */
};
