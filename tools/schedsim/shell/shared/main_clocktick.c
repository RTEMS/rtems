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

int rtems_shell_main_clock_tick(
  int   argc,
  char *argv[]
)
{
  rtems_status_code    status;
  rtems_interval       ticks;
  unsigned long        tmp;
  rtems_interval       t;
 
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

  /*
   *  Now delete the task
   */
  for ( t=1 ; t<=ticks ; t++ ) {
    fprintf( stderr, "ClockTick (%d) ...\n", t );
    status = rtems_clock_tick();
    if ( status != RTEMS_SUCCESSFUL ) {
      fprintf(
        stderr,
        "Clock Tick (%s) returned %s\n",
        argv[1],
        rtems_status_text( status )
      ); 
      return -1;
    }
  }

  return 0;
}

rtems_shell_cmd_t rtems_shell_CLOCK_TICK_Command = {
  "clock_tick",                  /* name */
  "clock_tick ticks",            /* usage */
  "rtems",                       /* topic */
  rtems_shell_main_clock_tick,   /* command */
  NULL,                          /* alias */
  NULL                           /* next */
};
