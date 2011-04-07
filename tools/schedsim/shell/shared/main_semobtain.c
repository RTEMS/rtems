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

int rtems_shell_main_semaphore_obtain(
  int   argc,
  char *argv[]
)
{
  rtems_id           id;
  rtems_status_code  status;
  long               tmp;
  rtems_interval     ticks;
  Thread_Control    *caller;
 
  /* XXX for now, do not support polling */

  CHECK_RTEMS_IS_UP();

  if (argc != 3) {
    fprintf( stderr, "%s: Usage name|id timeout\n", argv[0] );
    return -1;
  }

  if ( lookup_semaphore( argv[1], &id ) )
    return -1;

  if ( rtems_string_to_long(argv[2], &tmp, NULL, 0) ) {
    printf( "Ceiling argument (%s) is not a number\n", argv[1] );
    return -1;
  }
  ticks = tmp;

  /*
   *  Now obtain the semaphore
   *
   *  If the calling thread blocks, we will return as another thread
   *  but with a "unsatisfied" return code.  So we check that we did
   *  a thread switch inside the semaphore obtain.  If we did, then
   *  just return successfully.
   */
  caller = _Thread_Executing;
  printf("Obtain semaphore (0x%08x) with timeout %d\n", id, ticks );
  status = rtems_semaphore_obtain( id, RTEMS_DEFAULT_OPTIONS, ticks );
  if ( caller == _Thread_Executing ) {
    if ( status != RTEMS_SUCCESSFUL ) {
      fprintf(
        stderr,
        "Semaphore obtain(%s) returned %s\n",
        argv[1],
        rtems_status_text( status )
      ); 
      return -1;
    }
  }
  return 0;
}

rtems_shell_cmd_t rtems_shell_SEMAPHORE_OBTAIN_Command = {
  "semaphore_obtain",                /* name */
  "semaphore_obtain name ticks",     /* usage */
  "rtems",                           /* topic */
  rtems_shell_main_semaphore_obtain, /* command */
  NULL,                              /* alias */
  NULL                               /* next */
};
