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

int rtems_shell_main_semaphore_release(
  int   argc,
  char *argv[]
)
{
  rtems_id           id;
  rtems_status_code  status;
 
  CHECK_RTEMS_IS_UP();

  if (argc != 2) {
    fprintf( stderr, "%s: Usage [name|id]\n", argv[0] );
    return -1;
  }

  if ( lookup_semaphore( argv[1], &id ) )
    return -1;

  /*
   *  Now release the semaphore
   */
  printf("Releasing semaphore (0x%08x)\n", id );
  status = rtems_semaphore_release( id );
  if ( status != RTEMS_SUCCESSFUL ) {
    fprintf(
      stderr,
      "Semaphore release(%s) returned %s\n",
      argv[1],
      rtems_status_text( status )
    ); 
    return -1;
  }

  return 0;
}

rtems_shell_cmd_t rtems_shell_SEMAPHORE_RELEASE_Command = {
  "semaphore_release",                 /* name */
  "semaphore_release name",            /* usage */
  "rtems",                             /* topic */
  rtems_shell_main_semaphore_release,  /* command */
  NULL,                                /* alias */
  NULL                                 /* next */
};
