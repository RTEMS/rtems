/*
 *  RMDIR Shell Command Implmentation
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
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_rmdir (int argc, char *argv[])
{
  char *dir;
  int n;

  n = 1;
  while (n < argc) {
    dir = argv[n++];
    if (rmdir(dir)) {
      fprintf(stderr,"%s: %s: %s\n", argv[0], dir, strerror(errno));
      return -1;
    }
  }
  return 0;
}

rtems_shell_cmd_t rtems_shell_RMDIR_Command = {
  "rmdir",                                      /* name */
  "rmdir  dir   # remove directory",            /* usage */
  "files",                                      /* topic */
  rtems_shell_main_rmdir,                       /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
