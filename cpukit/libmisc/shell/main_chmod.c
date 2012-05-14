/*
 *  CHMOD Shell Command Implmentation
 *
 *  Author: Fernando RUIZ CASAS
 *  Work: fernando.ruiz@ctv.es
 *  Home: correo@fernando-ruiz.com
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
#include <sys/types.h>
#include <sys/stat.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/stringto.h>
#include "internal.h"

static int rtems_shell_main_chmod(
  int argc,
  char *argv[]
)
{
  int           n;
  mode_t        mode;
  unsigned long tmp;

  if (argc < 2) {
    fprintf(stderr,"%s: too few arguments\n", argv[0]);
    return -1;
  }

  /*
   *  Convert arguments into numbers
   */
  if ( rtems_string_to_unsigned_long(argv[1], &tmp, NULL, 0) ) {
    printf( "Mode argument (%s) is not a number\n", argv[1] );
    return -1;
  }
  mode = (mode_t) (tmp & 0777);

  /*
   *  Now change the files modes
   */
  for (n=2 ; n < argc ; n++)
    chmod(argv[n++], mode);

  return 0;
}

rtems_shell_cmd_t rtems_shell_CHMOD_Command = {
  "chmod",                                      /* name */
  "chmod 0777 n1 n2... # change filemode",      /* usage */
  "files",                                      /* topic */
  rtems_shell_main_chmod,                       /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
