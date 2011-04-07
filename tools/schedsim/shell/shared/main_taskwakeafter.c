/*
 *  Task Priority Shell Command Implmentation
 *
 *  COPYRIGHT (c) 1989-2010.
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
#include "shell.h"
#include <rtems/stringto.h>
#include <schedsim_shell.h>
#include <rtems/error.h>

int rtems_shell_main_task_wake_after(
  int   argc,
  char *argv[]
)
{
  rtems_status_code    status;
  rtems_interval       ticks;
  unsigned long        tmp;
  rtems_id             self;
 
  CHECK_RTEMS_IS_UP();

  if (argc != 2) {
    fprintf( stderr, "%s: Usage ticks\n", argv[0] );
    return -1;
  }

  if ( rtems_string_to_unsigned_long( argv[1], &tmp, NULL, 0) ) {
    fprintf( stderr, "Argument (%s) is not a number\n", argv[2] );
    return 1;
  }

  ticks = (rtems_interval) tmp;
  self = _Thread_Executing->Object.id,

  /*
   *  Now sleep
   */
  printf( "Task (0x%08x) sleeping for %d ticks\n", self, ticks );
  
  status = rtems_task_wake_after( ticks );
  if ( status != RTEMS_SUCCESSFUL ) {
    fprintf(
      stderr,
      "Task Wake After (%s) returned %s\n",
      argv[1],
      rtems_status_text( status )
    ); 
    return -1;
  }

  return 0;
}

rtems_shell_cmd_t rtems_shell_TASK_WAKE_AFTER_Command = {
  "task_wake_after",                  /* name */
  "task_wake_after ticks",            /* usage */
  "rtems",                            /* topic */
  rtems_shell_main_task_wake_after,   /* command */
  NULL,                               /* alias */
  NULL                                /* next */
};
