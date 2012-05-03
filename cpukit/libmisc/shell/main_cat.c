/*
 *  CAT Shell Command Implmentation
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
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <stddef.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_cat(int argc, char *argv[])
{
  int n;
  int sc;

  for ( n=1; n < argc ; n++) {
    sc = rtems_shell_cat_file(stdout, argv[n]);
    if ( sc == -1 ) {
      fprintf(stderr, "%s: %s: %s\n", argv[0], argv[n], strerror(errno));
      return -1;
    }
  }
  return 0;
}

rtems_shell_cmd_t rtems_shell_CAT_Command = {
  "cat",                                           /* name */
  "cat n1 [n2 [n3...]] # show the ascii contents", /* usage */
  "files",                                         /* topic */
  rtems_shell_main_cat ,                           /* command */
  NULL,                                            /* alias */
  NULL                                             /* next */
};
