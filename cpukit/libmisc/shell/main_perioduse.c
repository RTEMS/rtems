/*
 *  perioduse Command Implementation
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
#include <rtems/shell.h>
#include "internal.h"

int rtems_shell_main_perioduse(int argc,char *argv[])
{
  if ( argc >= 1 && !strcmp( argv[1], "-r" ) ) {
    printf( "Resetting Period Usage information\n" );
    rtems_rate_monotonic_reset_all_statistics();
  } else {
    rtems_rate_monotonic_report_statistics_with_plugin(
      stdout,
      (rtems_printk_plugin_t)fprintf
    );
  }
  return 0;
}

rtems_shell_cmd_t rtems_shell_PERIODUSE_Command = {
  "perioduse",                            /* name */
  "[-r] print or reset per period usage", /* usage */
  "rtems",                                /* topic */
  rtems_shell_main_perioduse,             /* command */
  NULL,                                   /* alias */
  NULL                                    /* next */
};
