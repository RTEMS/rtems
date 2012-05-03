/*
 *  DATE Shell Command Implmentation
 *
 *  OAuthor: Fernando RUIZ CASAS
 *  Work: fernando.ruiz@ctv.es
 *  Home: correo@fernando-ruiz.com
 *
 *  Significantly rewritten by Joel Sherrill <joel.sherrill@oarcorp.com>.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_date(
  int   argc,
  char *argv[]
)
{
  /*
   *  Print the current date and time in default format.
   */
  if ( argc == 1 ) {
    time_t t;

    time(&t);
    printf("%s", ctime(&t));
    return 0;
  }

  /*
   *  Set the current date and time
   */
  if ( argc == 3 ) {
    char buf[128];
    struct tm TOD;
    struct timespec timesp;
    char *result;

    snprintf( buf, sizeof(buf), "%s %s", argv[1], argv[2] );
    result = strptime(
      buf,
      "%Y-%m-%d %T",
      &TOD
    );
    if ( result && !*result ) {
      timesp.tv_sec = mktime( &TOD );
      timesp.tv_nsec = 0;
      clock_settime( CLOCK_REALTIME, &timesp );
      return 0;
    }
  }

  fprintf( stderr, "%s: Usage: [YYYY-MM-DD HH:MM:SS]\n", argv[0] );
  return -1;
}

rtems_shell_cmd_t rtems_shell_DATE_Command = {
  "date",                        /* name */
  "date [YYYY-MM-DD HH:MM:SS]",  /* usage */
  "misc",                        /* topic */
  rtems_shell_main_date,         /* command */
  NULL,                          /* alias */
  NULL                           /* next */
};
