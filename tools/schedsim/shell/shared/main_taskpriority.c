/*
 *  Task Delete Shell Command Implmentation
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

int rtems_shell_main_task_priority(
  int   argc,
  char *argv[]
)
{
  rtems_id             id;
  rtems_status_code    status;
  unsigned long        tmp;
  rtems_task_priority  old;
  rtems_task_priority  new;
 
  CHECK_RTEMS_IS_UP();

  if (argc != 3) {
    fprintf( stderr, "%s: Usage [name|id] priority\n", argv[0] );
    return -1;
  }

  if ( lookup_task( argv[1], &id ) )
    return -1;

  if ( rtems_string_to_unsigned_long( argv[2], &tmp, NULL, 0) ) {
    fprintf( stderr, "Argument (%s) is not a number\n", argv[2] );
    return 1;
  }

  new = (rtems_task_priority) tmp;

  /*
   *  Now priority the task
   */
  status = rtems_task_set_priority( id, new, &old );
  if ( status != RTEMS_SUCCESSFUL ) {
    fprintf(
      stderr,
      "Task Set Priority(%s) returned %s\n",
      argv[1],
      rtems_status_text( status )
    ); 
    return -1;
  }

  if ( new != 0 )
    printf("Task (0x%08x) Change Priority from %d to %d\n", id, old, new );
  else
    printf("Task (0x%08x) Current Priority is %d\n", id, new );
  
  return 0;
}

rtems_shell_cmd_t rtems_shell_TASK_PRIORITY_Command = {
  "task_priority",                 /* name */
  "task_priority name priority",   /* usage */
  "rtems",                         /* topic */
  rtems_shell_main_task_priority,  /* command */
  NULL,                            /* alias */
  NULL                             /* next */
};
