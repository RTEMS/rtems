/*
 *  CHDIR Shell Command Implmentation
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

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_chdir(
  int   argc,
  char *argv[]
)
{
  char *dir;

  dir = "/";

  if (argc > 1)
    dir = argv[1];

  if (chdir(dir)) {
    fprintf(stderr, "chdir to '%s' failed:%s\n", dir,strerror(errno));
    return errno;
  }
  return 0;
}

rtems_shell_cmd_t rtems_shell_CHDIR_Command = {
  "chdir",                                        /* name */
  "chdir [dir]  # change the current directory",  /* usage */
  "files",                                        /* topic */
  rtems_shell_main_chdir,                         /* command */
  NULL,                                           /* alias */
  NULL                                            /* next */
};
