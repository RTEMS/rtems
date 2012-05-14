/*
 *  CPUUSE Command Implementation
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include <rtems.h>
#include <rtems/cpuuse.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_cpuuse(
  int   argc,
  char *argv[]
)
{
  /*
   *  When invoked with no arguments, print the report.
   */
  if ( argc == 1 ) {
    rtems_cpu_usage_report_with_plugin(stdout, (rtems_printk_plugin_t)fprintf);
    return 0;
  }

  /*
   *  When invoked with the single argument -r, reset the statistics.
   */
  if ( argc == 2 && !strcmp( argv[1], "-r" ) ) {
    printf( "Resetting CPU Usage information\n" );
    rtems_cpu_usage_reset();
    return 0;
  }

  /*
   *  OK.  The user did something wrong.
   */
  fprintf( stderr, "%s: [-r]\n", argv[0] );
  return -1;
}

rtems_shell_cmd_t rtems_shell_CPUUSE_Command = {
  "cpuuse",                                   /* name */
  "[-r] print or reset per thread cpu usage", /* usage */
  "rtems",                                    /* topic */
  rtems_shell_main_cpuuse,                    /* command */
  NULL,                                       /* alias */
  NULL                                        /* next */
};
