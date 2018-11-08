/*
 *  MALLOC_INFO Shell Command Implmentation
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>
#include <string.h>

#include <rtems.h>
#include <rtems/malloc.h>
#include <rtems/libcsupport.h>
#include <rtems/shellconfig.h>

#include "internal.h"

static int rtems_shell_main_malloc_info(
  int   argc,
  char *argv[]
)
{
  if ( argc == 2 && strcmp( argv[ 1 ], "walk" ) == 0 ) {
    malloc_walk( 0, true );
  } else {
    Heap_Information_block info;

    rtems_shell_print_unified_work_area_message();
    malloc_info( &info );
    rtems_shell_print_heap_info( "free", &info.Free );
    rtems_shell_print_heap_info( "used", &info.Used );
    rtems_shell_print_heap_stats( &info.Stats );
  }

  return 0;
}

rtems_shell_cmd_t rtems_shell_MALLOC_INFO_Command = {
  "malloc",                                   /* name */
  "malloc [walk]",                            /* usage */
  "mem",                                      /* topic */
  rtems_shell_main_malloc_info,               /* command */
  NULL,                                       /* alias */
  NULL                                        /* next */
};

