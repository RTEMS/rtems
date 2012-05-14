/*
 *  MKDIR Shell Command Implmentation
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
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_mkdir(
  int   argc,
  char *argv[]
)
{
  char *dir;
  int n;

  n = 1;
  while (n<argc) {
    dir = argv[n++];
    if (mkdir(dir,S_IRWXU|S_IRWXG|S_IRWXO)) {
      fprintf(stderr, "mkdir '%s' failed:%s\n", dir, strerror(errno));
    }
  }
  return errno;
}

rtems_shell_cmd_t rtems_shell_MKDIR_Command = {
  "mkdir",                                      /* name */
  "mkdir  dir   # make a directory",            /* usage */
  "files",                                      /* topic */
  rtems_shell_main_mkdir,                       /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
