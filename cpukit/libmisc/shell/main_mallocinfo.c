/*
 *  MALLOC_INFO Shell Command Implmentation
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

#include <inttypes.h>

#include <rtems.h>
#include <rtems/malloc.h>
#include <rtems/shell.h>
#include "internal.h"

extern int malloc_info( region_information_block * );

static void printit(
  const char       *c,
  Heap_Information *h
)
{
  printf(
    "Number of %s blocks: %" PRId32 "\n"
    "Largest %s block:    %" PRId32 "\n"
    "Total bytes %s:      %" PRId32 "\n",
    c, h->number,
    c, h->largest,
    c, h->total
  );
}

int rtems_shell_main_malloc_info(
  int   argc,
  char *argv[]
)
{
  if ( argc == 2 ) {
    if ( !strcmp( argv[1], "info" ) ) {
      region_information_block info;

      malloc_info( &info );
      printit( "free", &info.Free );
      printit( "used", &info.Used );
      return 0;
    } else if ( !strcmp( argv[1], "stats" ) ) {
      malloc_report_statistics_with_plugin(
        stdout,
        (rtems_printk_plugin_t) fprintf
      );
      return 0;
    }
  }
  fprintf( stderr, "subcommands info or stats\n" );
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

