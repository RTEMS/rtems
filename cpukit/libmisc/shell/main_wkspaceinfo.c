/*
 *  MALLOC_INFO Shell Command Implmentation
 *
 *  COPYRIGHT (c) 1989-2008.
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
#include <rtems/score/protectedheap.h>
#include "internal.h"

int rtems_shell_main_wkspace_info(
  int   argc,
  char *argv[]
)
{
  Heap_Information_block info;

  _Protected_heap_Get_information( &_Workspace_Area, &info );
  rtems_shell_print_heap_info( "free", &info.Free );
  rtems_shell_print_heap_info( "used", &info.Used );

  return 0;
}

rtems_shell_cmd_t rtems_shell_WKSPACE_INFO_Command = {
  "wkspace",                                  /* name */
  "Report on RTEMS Executive Workspace",      /* usage */
  "rtems",                                    /* topic */
  rtems_shell_main_wkspace_info,              /* command */
  NULL,                                       /* alias */
  NULL                                        /* next */
};

