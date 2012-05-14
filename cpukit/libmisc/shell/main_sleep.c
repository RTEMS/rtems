/*
 *  Sleep Shell Command Implmentation
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <time.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/stringto.h>
#include "internal.h"

static int rtems_shell_main_sleep(
  int   argc,
  char *argv[]
)
{
  struct timespec delay;
  unsigned long   tmp;

  if ((argc != 2) && (argc != 3)) {
    fprintf( stderr, "%s: Usage seconds [nanoseconds]\n", argv[0] );
    return -1;
  }

  /*
   *  Convert the seconds argument to a number
   */
  if ( rtems_string_to_unsigned_long(argv[1], &tmp, NULL, 0) ) {
    printf( "Seconds argument (%s) is not a number\n", argv[1] );
    return -1;
  }
  delay.tv_sec = (time_t) tmp;

  /*
   *  If the user specified a nanoseconds argument, convert it
   */
  delay.tv_nsec = 0;
  if (argc == 3) {
    if ( rtems_string_to_unsigned_long(argv[2], &tmp, NULL, 0) ) {
      printf( "Seconds argument (%s) is not a number\n", argv[1] );
      return -1;
    }
    delay.tv_nsec = tmp;
  }

  /*
   *  Now sleep as requested.
   */
  nanosleep( &delay, NULL );
  return 0;
}

rtems_shell_cmd_t rtems_shell_SLEEP_Command = {
  "sleep",                       /* name */
  "sleep seconds [nanoseconds]", /* usage */
  "misc",                        /* topic */
  rtems_shell_main_sleep,        /* command */
  NULL,                          /* alias */
  NULL                           /* next */
};
