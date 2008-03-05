/*
 *  Sleep Shell Command Implmentation
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <time.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

int rtems_shell_main_sleep(
  int   argc,
  char *argv[]
)
{
  struct timespec delay;

  if (argc == 2) {
    delay.tv_sec = rtems_shell_str2int(argv[1]);
    delay.tv_nsec = 0;
    nanosleep( &delay, NULL );
    return 0;
  }
  
  if (argc == 3) {
    delay.tv_sec = rtems_shell_str2int(argv[1]);
    delay.tv_nsec = rtems_shell_str2int(argv[2]);
    nanosleep( &delay, NULL );
    return 0;
  }
  
  fprintf( stderr, "%s: Usage seconds [nanoseconds]\n", argv[0] );
  return -1;
}

rtems_shell_cmd_t rtems_shell_SLEEP_Command = {
  "sleep",                       /* name */
  "sleep seconds [nanoseconds]", /* usage */
  "misc",                        /* topic */
  rtems_shell_main_sleep,        /* command */
  NULL,                          /* alias */
  NULL                           /* next */
};
