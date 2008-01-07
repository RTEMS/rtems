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

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>

#include <rtems.h>
#include <rtems/malloc.h>
#include <rtems/shell.h>
#include "internal.h"

int rtems_shell_main_wkspace_info(
  int   argc,
  char *argv[]
)
{
  Heap_Information_block info;
  extern void classinfo_tester();

  /* XXX lock allocator and do not violate visibility */
  _Heap_Get_information( &_Workspace_Area, &info );
  rtems_shell_print_heap_info( "free", &info.Free );
  rtems_shell_print_heap_info( "used", &info.Used );

  classinfo_tester();
  decode_id( _Thread_Executing->Object.id );
  return 0;
}

rtems_shell_cmd_t rtems_shell_WKSPACE_INFO_Command = {
  "wkspace",                                  /* name */
  "",                                         /* usage */
  "rtems",                                    /* topic */
  rtems_shell_main_wkspace_info,              /* command */
  NULL,                                       /* alias */
  NULL                                        /* next */
};

