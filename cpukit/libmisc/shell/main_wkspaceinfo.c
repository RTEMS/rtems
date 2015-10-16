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

#include <rtems.h>
#include <rtems/malloc.h>
#include <rtems/shell.h>
#include <rtems/score/protectedheap.h>
#include <rtems/score/wkspace.h>
#include "internal.h"

void rtems_shell_print_unified_work_area_message(void)
{
  printf( "C Program Heap and RTEMS Workspace are %s.\n",
    rtems_configuration_get_unified_work_area() ? "the same" : "separate"
  );
}

static int rtems_shell_main_wkspace_info(
  int   argc RTEMS_UNUSED,
  char *argv[] RTEMS_UNUSED
)
{
  Heap_Information_block info;

  rtems_shell_print_unified_work_area_message();

  _Protected_heap_Get_information( &_Workspace_Area, &info );
  rtems_shell_print_heap_info( "free", &info.Free );
  rtems_shell_print_heap_info( "used", &info.Used );
  rtems_shell_print_heap_stats( &info.Stats );

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
