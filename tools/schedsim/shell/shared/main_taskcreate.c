/*
 *  Task Create Shell Command Implmentation
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

rtems_task dummy_task(
  rtems_task_argument arg
)
{
}

int rtems_shell_main_task_create(
  int   argc,
  char *argv[]
)
{
  char               name[5];
  rtems_id           id;
  rtems_status_code  status;
  long               priority;
 
  CHECK_RTEMS_IS_UP();

  if (argc != 3) {
    fprintf( stderr, "%s: Usage name priority\n", argv[0] );
    return -1;
  }

  if ( rtems_string_to_long(argv[2], &priority, NULL, 0) ) {
    printf( "Seconds argument (%s) is not a number\n", argv[1] );
    return -1;
  }

  /*
   *  Now create the task
   */
  memset( name, '\0', sizeof(name) );
  strncpy( name, argv[1], 4 );

  status = rtems_task_create(
    rtems_build_name( name[0], name[1], name[2], name[3] ),
    (rtems_task_priority) priority,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  if ( status ) {
    fprintf(
      stderr,
      "Task Create(%s) returned %s\n",
      argv[1],
      rtems_status_text( status )
    ); 
    return -1;
  }

  printf(
    "Task (%s) created: id=0x%08x, priority=%ld\n",
    argv[1],
    id,
    priority
  );
  
  printf(
    "Task (%s) starting: id=0x%08x, priority=%ld\n",
    argv[1],
    id,
    priority
  );

  status = rtems_task_start( id, dummy_task, 1 );
  if ( status ) {
    fprintf(
      stderr,
      "Task Start(%s) returned %s\n",
      argv[1],
      rtems_status_text( status )
    ); 
    return -1;
  }

  return 0;
}

rtems_shell_cmd_t rtems_shell_TASK_CREATE_Command = {
  "task_create",                 /* name */
  "task_create name priority",   /* usage */
  "rtems",                       /* topic */
  rtems_shell_main_task_create,  /* command */
  NULL,                          /* alias */
  NULL                           /* next */
};
