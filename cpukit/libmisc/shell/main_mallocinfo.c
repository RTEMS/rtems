/*
 *  MALLOC_INFO Shell Command Implmentation
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

#include <inttypes.h>

#include <rtems.h>
#include <rtems/malloc.h>
#include <rtems/libcsupport.h>
#include <rtems/shell.h>

#include "internal.h"

static int rtems_shell_main_malloc_info(
  int   argc,
  char *argv[]
)
{
  if ( argc == 2 ) {
    rtems_shell_print_unified_work_area_message();

    if ( !strcmp( argv[1], "info" ) ) {
      region_information_block info;

      malloc_info( &info );
      rtems_shell_print_heap_info( "free", &info.Free );
      rtems_shell_print_heap_info( "used", &info.Used );
      return 0;
    } else if ( !strcmp( argv[1], "stats" ) ) {
      malloc_report_statistics_with_plugin(
        stdout,
        (rtems_printk_plugin_t) fprintf
      );
      return 0;
    }
  }
  fprintf( stderr, "%s: [info|stats]\n", argv[0] );
  return -1;
}

rtems_shell_cmd_t rtems_shell_MALLOC_INFO_Command = {
  "malloc",                                   /* name */
  "[info|stats]",                             /* usage */
  "mem",                                      /* topic */
  rtems_shell_main_malloc_info,               /* command */
  NULL,                                       /* alias */
  NULL                                        /* next */
};

